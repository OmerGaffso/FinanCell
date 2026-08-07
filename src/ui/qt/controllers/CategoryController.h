#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

class CategoryService;
class CellService;
class SessionState;

/** @brief Qt-facing adapter for cell-scoped category operations. */
class CategoryController final : public QObject
{
    Q_OBJECT
    /** @brief Categories returned by the latest load. */
    Q_PROPERTY(QVariantList categories READ categories NOTIFY categoriesChanged)
    /** @brief Whether the active member may create categories. */
    Q_PROPERTY(bool canCreate READ canCreate NOTIFY categoriesChanged)
    /** @brief Latest category-operation error. */
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    /** @brief Creates the controller. @param categoryService Category service. @param cellService Cell service used for role presentation. @param session Shared GUI session. @param parent Optional Qt owner. */
    CategoryController(
        CategoryService& categoryService,
        CellService& cellService,
        SessionState& session,
        QObject* parent = nullptr);

    /** @brief Returns loaded categories. @return Category maps for QML. */
    QVariantList categories() const;
    /** @brief Returns whether category creation is available. @return True for Manager or Member. */
    bool canCreate() const;
    /** @brief Returns the latest failure. @return Error text, or empty. */
    QString errorMessage() const;

    /** @brief Loads categories for an accessible cell. @param cellId Cell ID. @return True when loaded. */
    Q_INVOKABLE bool loadCategories(qulonglong cellId);
    /** @brief Creates a category. @param cellId Cell ID. @param name Category name. @return True on success. */
    Q_INVOKABLE bool createCategory(qulonglong cellId, const QString& name);
    /** @brief Sets a category's recurring monthly budget. @param cellId Cell ID. @param categoryId Category ID. @param amount Decimal major-unit amount. @return True when saved. */
    Q_INVOKABLE bool setMonthlyBudget(
        qulonglong cellId,
        qulonglong categoryId,
        const QString& amount);
    /** @brief Clears a category's recurring monthly budget. @param cellId Cell ID. @param categoryId Category ID. @return True when cleared. */
    Q_INVOKABLE bool clearMonthlyBudget(
        qulonglong cellId,
        qulonglong categoryId);
    /** @brief Clears the latest error. */
    Q_INVOKABLE void clearError();

signals:
    /** @brief Emitted when categories or creation access change. */
    void categoriesChanged();
    /** @brief Emitted when the user-facing error changes. */
    void errorMessageChanged();

private:
    void clearCategories();
    void setErrorMessage(const QString& message);

    CategoryService& m_categoryService;
    CellService& m_cellService;
    SessionState& m_session;
    QVariantList m_categories;
    bool m_canCreate{false};
    QString m_errorMessage;
};
