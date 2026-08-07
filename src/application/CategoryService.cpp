#include "application/CategoryService.h"

#include "application/PersistenceError.h"
#include "utils/StringUtils.h"

CategoryService::CategoryService(
    CategoryRepository& categoryRepository,
    CellRepository& cellRepository)
    : m_categoryRepository(categoryRepository), m_cellRepository(cellRepository)
{
}

CategoryOperationResult CategoryService::createCategory(
    std::uint64_t actingUserId,
    std::uint64_t cellId,
    const std::string& name)
{
    if (!m_cellRepository.findCellById(cellId))
        return CategoryOperationResult::CELL_NOT_FOUND;

    const auto membership = m_cellRepository.findMember(cellId, actingUserId);
    if (!membership || membership->role == CellRole::GUEST)
        return CategoryOperationResult::NOT_AUTHORIZED;

    const std::string trimmedName = StringUtils::trim(name);
    if (!isNameValid(trimmedName)) return CategoryOperationResult::INVALID_INPUT;
    if (m_categoryRepository.findCategoryByName(cellId, trimmedName))
        return CategoryOperationResult::ALREADY_EXISTS;

    try
    {
        return m_categoryRepository.insertCategory(Category(0, cellId, trimmedName))
            ? CategoryOperationResult::SUCCESS
            : CategoryOperationResult::STORAGE_ERROR;
    }
    catch (const PersistenceError&)
    {
        return CategoryOperationResult::STORAGE_ERROR;
    }
}

CategoryOperationResult CategoryService::setMonthlyBudget(
    std::uint64_t actingUserId,
    std::uint64_t cellId,
    std::uint64_t categoryId,
    std::int64_t amountInMinorUnits)
{
    if (!m_cellRepository.findCellById(cellId))
        return CategoryOperationResult::CELL_NOT_FOUND;

    const auto membership = m_cellRepository.findMember(cellId, actingUserId);
    if (!membership || membership->role == CellRole::GUEST)
        return CategoryOperationResult::NOT_AUTHORIZED;

    const auto category = m_categoryRepository.findCategoryById(categoryId);
    if (!category || category->getCellId() != cellId)
        return CategoryOperationResult::CATEGORY_NOT_FOUND;
    if (amountInMinorUnits < 0)
        return CategoryOperationResult::INVALID_INPUT;

    try
    {
        return m_categoryRepository.updateCategoryBudget(categoryId, amountInMinorUnits)
            ? CategoryOperationResult::SUCCESS
            : CategoryOperationResult::STORAGE_ERROR;
    }
    catch (const PersistenceError&)
    {
        return CategoryOperationResult::STORAGE_ERROR;
    }
}

std::optional<std::vector<Category>> CategoryService::getCategoriesForCell(
    std::uint64_t actingUserId,
    std::uint64_t cellId) const
{
    if (!m_cellRepository.findMember(cellId, actingUserId)) return std::nullopt;
    return m_categoryRepository.findCategoriesByCellId(cellId);
}

bool CategoryService::isNameValid(const std::string& name)
{
    const std::string trimmedName = StringUtils::trim(name);
    return !trimmedName.empty() && trimmedName.size() <= MAX_CATEGORY_NAME_LENGTH;
}
