#include "ui/qt/controllers/CategoryController.h"

#include <QDebug>
#include <QVariantMap>

#include <cstdint>
#include <exception>
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
        for (const auto& category : *categories)
        {
            QVariantMap value;
            value.insert(QStringLiteral("categoryId"),
                         QVariant::fromValue<qulonglong>(category.getCategoryId()));
            value.insert(QStringLiteral("name"),
                         QString::fromStdString(category.getName()));
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
