#pragma once

#include "application/CategoryRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"

/** @brief SQLite implementation of category persistence. */
class SQLiteCategoryRepository final : public CategoryRepository
{
public:
    /** @brief Creates the repository. @param database Shared database connection. */
    explicit SQLiteCategoryRepository(SQLiteDatabase& database);

    /** @copydoc CategoryRepository::insertCategory */
    std::optional<Category> insertCategory(const Category& category) override;
    /** @copydoc CategoryRepository::findCategoryById */
    std::optional<Category> findCategoryById(std::uint64_t categoryId) const override;
    /** @copydoc CategoryRepository::findCategoryByName */
    std::optional<Category> findCategoryByName(
        std::uint64_t cellId,
        const std::string& name) const override;
    /** @copydoc CategoryRepository::findCategoriesByCellId */
    std::vector<Category> findCategoriesByCellId(std::uint64_t cellId) const override;

private:
    SQLiteDatabase& m_database;
};
