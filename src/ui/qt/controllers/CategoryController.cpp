#include "ui/qt/controllers/CategoryController.h"

#include <QDebug>
#include <QVariantMap>

#include <cstdint>
#include <exception>
#include <iomanip>
#include <limits>
#include <optional>
#include <sstream>
#include <utility>

#include "application/CategoryService.h"
#include "application/CellService.h"
#include "ui/qt/session/SessionState.h"

namespace
{
QString categoryResultMessage(CategoryOperationResult result)
{
    switch (result)
    {
        case CategoryOperationResult::SUCCESS: return {};
        case CategoryOperationResult::CELL_NOT_FOUND:
            return QStringLiteral("The financial cell no longer exists.");
        case CategoryOperationResult::CATEGORY_NOT_FOUND:
            return QStringLiteral("The selected category no longer exists.");
        case CategoryOperationResult::INVALID_INPUT:
            return QStringLiteral("Category names must contain 1 to 50 characters.");
        case CategoryOperationResult::ALREADY_EXISTS:
            return QStringLiteral("That category already exists in this cell.");
        case CategoryOperationResult::NOT_AUTHORIZED:
            return QStringLiteral("Guests cannot create categories.");
        case CategoryOperationResult::STORAGE_ERROR:
            return QStringLiteral("The category could not be saved.");
    }
    return QStringLiteral("The category request could not be completed.");
}

std::optional<std::int64_t> parsePositiveAmount(const QString& input)
{
    const std::string text = input.trimmed().toStdString();
    if (text.empty()) return std::nullopt;
    const std::size_t dot = text.find('.');
    if (dot != std::string::npos && text.find('.', dot + 1) != std::string::npos)
        return std::nullopt;

    const std::string wholeText = dot == std::string::npos ? text : text.substr(0, dot);
    const std::string fractionText = dot == std::string::npos ? "" : text.substr(dot + 1);
    if (wholeText.empty() || fractionText.size() > 2) return std::nullopt;
    for (char value : wholeText)
        if (value < '0' || value > '9') return std::nullopt;
    for (char value : fractionText)
        if (value < '0' || value > '9') return std::nullopt;

    try
    {
        const std::uint64_t whole = std::stoull(wholeText);
        const std::uint64_t fraction = fractionText.empty()
            ? 0
            : static_cast<std::uint64_t>(std::stoul(fractionText)) *
                  (fractionText.size() == 1 ? 10 : 1);
        const auto maximum = static_cast<std::uint64_t>(
            std::numeric_limits<std::int64_t>::max());
        if (whole > (maximum - fraction) / 100) return std::nullopt;
        const std::uint64_t total = whole * 100 + fraction;
        if (total == 0) return std::nullopt;
        return static_cast<std::int64_t>(total);
    }
    catch (const std::exception&)
    {
        return std::nullopt;
    }
}

QString formatMoney(std::int64_t amount, const std::string& currency)
{
    std::ostringstream output;
    output << amount / 100 << '.' << std::setw(2) << std::setfill('0')
           << amount % 100 << ' ' << currency;
    return QString::fromStdString(output.str());
}

QString amountInput(std::int64_t amount)
{
    std::ostringstream output;
    output << amount / 100 << '.' << std::setw(2) << std::setfill('0')
           << amount % 100;
    return QString::fromStdString(output.str());
}
}

CategoryController::CategoryController(
    CategoryService& categoryService,
    CellService& cellService,
    SessionState& session,
    QObject* parent)
    : QObject(parent),
      m_categoryService(categoryService),
      m_cellService(cellService),
      m_session(session)
{
    connect(&m_session, &SessionState::sessionChanged, this, [this]()
    {
        clearCategories();
        clearError();
    });
}

QVariantList CategoryController::categories() const { return m_categories; }
bool CategoryController::canCreate() const { return m_canCreate; }
QString CategoryController::errorMessage() const { return m_errorMessage; }

bool CategoryController::loadCategories(qulonglong cellId)
{
    clearError();
    if (!m_session.loggedIn() || cellId == 0)
    {
        clearCategories();
        setErrorMessage(QStringLiteral("Open a financial cell to view categories."));
        return false;
    }

    try
    {
        const auto categories = m_categoryService.getCategoriesForCell(
            m_session.userId(), static_cast<std::uint64_t>(cellId));
        if (!categories)
        {
            clearCategories();
            setErrorMessage(QStringLiteral("This cell is not available to the active user."));
            return false;
        }

        QVariantList values;
        const auto cell = m_cellService.getCellForUser(
            m_session.userId(), static_cast<std::uint64_t>(cellId));
        const std::string currency = cell ? cell->getCurrency() : "ILS";
        for (const auto& category : *categories)
        {
            QVariantMap value;
            value.insert(QStringLiteral("categoryId"),
                         QVariant::fromValue<qulonglong>(category.getCategoryId()));
            value.insert(QStringLiteral("name"),
                         QString::fromStdString(category.getName()));
            const std::int64_t budget = category.getMonthlyBudgetInMinorUnits();
            value.insert(QStringLiteral("budgetInMinorUnits"),
                         QVariant::fromValue<qlonglong>(budget));
            value.insert(QStringLiteral("hasBudget"), budget > 0);
            value.insert(QStringLiteral("budgetText"),
                         budget > 0 ? formatMoney(budget, currency)
                                    : QStringLiteral("No monthly budget"));
            value.insert(QStringLiteral("budgetInput"),
                         budget > 0 ? amountInput(budget) : QString());
            values.append(value);
        }

        bool canCreate = false;
        for (const auto& member : m_cellService.getCellMembers(
                 m_session.userId(), static_cast<std::uint64_t>(cellId)))
        {
            if (member.userId == m_session.userId())
                canCreate = member.role != CellRole::GUEST;
        }
        m_categories = std::move(values);
        m_canCreate = canCreate;
        emit categoriesChanged();
        return true;
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Category loading failed:" << error.what();
        clearCategories();
        setErrorMessage(QStringLiteral(
            "Categories could not be loaded because the database operation failed."));
        return false;
    }
}

bool CategoryController::setMonthlyBudget(
    qulonglong cellId,
    qulonglong categoryId,
    const QString& amount)
{
    clearError();
    const auto parsedAmount = parsePositiveAmount(amount);
    if (!parsedAmount)
    {
        setErrorMessage(QStringLiteral(
            "Enter a positive monthly budget with no more than two decimal places."));
        return false;
    }

    try
    {
        const auto result = m_categoryService.setMonthlyBudget(
            m_session.userId(), cellId, categoryId, *parsedAmount);
        if (result != CategoryOperationResult::SUCCESS)
        {
            setErrorMessage(categoryResultMessage(result));
            return false;
        }
        return loadCategories(cellId);
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Category budget update failed:" << error.what();
        setErrorMessage(QStringLiteral("The monthly budget could not be saved."));
        return false;
    }
}

bool CategoryController::clearMonthlyBudget(
    qulonglong cellId,
    qulonglong categoryId)
{
    clearError();
    try
    {
        const auto result = m_categoryService.setMonthlyBudget(
            m_session.userId(), cellId, categoryId, 0);
        if (result != CategoryOperationResult::SUCCESS)
        {
            setErrorMessage(categoryResultMessage(result));
            return false;
        }
        return loadCategories(cellId);
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Category budget clearing failed:" << error.what();
        setErrorMessage(QStringLiteral("The monthly budget could not be cleared."));
        return false;
    }
}

bool CategoryController::createCategory(qulonglong cellId, const QString& name)
{
    clearError();
    try
    {
        const auto result = m_categoryService.createCategory(
            m_session.userId(),
            static_cast<std::uint64_t>(cellId),
            name.toStdString());
        if (result != CategoryOperationResult::SUCCESS)
        {
            setErrorMessage(categoryResultMessage(result));
            return false;
        }
        return loadCategories(cellId);
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Category creation failed:" << error.what();
        setErrorMessage(QStringLiteral("The category could not be saved."));
        return false;
    }
}

void CategoryController::clearError() { setErrorMessage({}); }

void CategoryController::clearCategories()
{
    if (m_categories.isEmpty() && !m_canCreate) return;
    m_categories.clear();
    m_canCreate = false;
    emit categoriesChanged();
}

void CategoryController::setErrorMessage(const QString& message)
{
    if (m_errorMessage == message) return;
    m_errorMessage = message;
    emit errorMessageChanged();
}
