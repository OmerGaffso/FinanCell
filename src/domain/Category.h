#pragma once

#include <cstdint>
#include <string>
#include <utility>

/** @brief Reusable transaction category scoped to one financial cell. */
class Category
{
public:
    /** @brief Constructs a category. @param id Category ID. @param cellId Owning cell ID. @param name Category name. @param monthlyBudgetInMinorUnits Optional recurring monthly budget, or zero when unset. */
    Category(
        std::uint64_t id,
        std::uint64_t cellId,
        std::string name,
        std::int64_t monthlyBudgetInMinorUnits = 0)
        : m_id(id),
          m_cellId(cellId),
          m_name(std::move(name)),
          m_monthlyBudgetInMinorUnits(monthlyBudgetInMinorUnits)
    {
    }

    /** @brief Returns the category ID. @return Category ID. */
    std::uint64_t getCategoryId() const { return m_id; }
    /** @brief Returns the owning cell ID. @return Cell ID. */
    std::uint64_t getCellId() const { return m_cellId; }
    /** @brief Returns the category name. @return Category name. */
    const std::string& getName() const { return m_name; }
    /** @brief Returns the recurring monthly budget. @return Positive minor-unit budget, or zero when unset. */
    std::int64_t getMonthlyBudgetInMinorUnits() const
    {
        return m_monthlyBudgetInMinorUnits;
    }

private:
    std::uint64_t m_id;
    std::uint64_t m_cellId;
    std::string m_name;
    std::int64_t m_monthlyBudgetInMinorUnits;
};
