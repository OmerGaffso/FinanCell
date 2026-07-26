#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "domain/Category.h"

/** @brief Persistence contract for cell-scoped categories. */
class CategoryRepository
{
public:
    /** @brief Destroys the category repository interface. */
    virtual ~CategoryRepository() = default;

    /** @brief Inserts a category. @param category Category to persist. @return Persisted category. */
    virtual std::optional<Category> insertCategory(const Category& category) = 0;
    /** @brief Finds a category by ID. @param categoryId Category ID. @return Matching category. */
    virtual std::optional<Category> findCategoryById(std::uint64_t categoryId) const = 0;
    /** @brief Finds a category by cell and name. @param cellId Cell ID. @param name Category name. @return Matching category. */
    virtual std::optional<Category> findCategoryByName(
        std::uint64_t cellId,
        const std::string& name) const = 0;
    /** @brief Lists categories in a cell. @param cellId Cell ID. @return Cell categories. */
    virtual std::vector<Category> findCategoriesByCellId(std::uint64_t cellId) const = 0;
};
