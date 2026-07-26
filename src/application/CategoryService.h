#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "application/CategoryRepository.h"
#include "application/CellRepository.h"

/** @brief Outcome of a category creation request. */
enum class CategoryOperationResult
{
    SUCCESS,
    CELL_NOT_FOUND,
    INVALID_INPUT,
    ALREADY_EXISTS,
    NOT_AUTHORIZED,
    STORAGE_ERROR
};

/** @brief Validates and authorizes cell-scoped category operations. */
class CategoryService
{
public:
    /** @brief Maximum accepted category-name length. */
    static constexpr std::size_t MAX_CATEGORY_NAME_LENGTH = 50;

    /** @brief Creates the service. @param categoryRepository Category persistence. @param cellRepository Cell persistence. */
    CategoryService(CategoryRepository& categoryRepository, CellRepository& cellRepository);

    /** @brief Creates a category. @param actingUserId Actor ID. @param cellId Cell ID. @param name Category name. @return Operation result. */
    CategoryOperationResult createCategory(
        std::uint64_t actingUserId,
        std::uint64_t cellId,
        const std::string& name);
    /** @brief Lists categories visible to a cell member. @param actingUserId Actor ID. @param cellId Cell ID. @return Categories, or empty optional when unauthorized. */
    std::optional<std::vector<Category>> getCategoriesForCell(
        std::uint64_t actingUserId,
        std::uint64_t cellId) const;
    /** @brief Checks a category name. @param name Name. @return True when valid. */
    static bool isNameValid(const std::string& name);

private:
    CategoryRepository& m_categoryRepository;
    CellRepository& m_cellRepository;
};
