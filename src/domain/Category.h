#pragma once

#include <cstdint>
#include <string>
#include <utility>

/** @brief Reusable transaction category scoped to one financial cell. */
class Category
{
public:
    /** @brief Constructs a category. @param id Category ID. @param cellId Owning cell ID. @param name Category name. */
    Category(std::uint64_t id, std::uint64_t cellId, std::string name)
        : m_id(id), m_cellId(cellId), m_name(std::move(name))
    {
    }

    /** @brief Returns the category ID. @return Category ID. */
    std::uint64_t getCategoryId() const { return m_id; }
    /** @brief Returns the owning cell ID. @return Cell ID. */
    std::uint64_t getCellId() const { return m_cellId; }
    /** @brief Returns the category name. @return Category name. */
    const std::string& getName() const { return m_name; }

private:
    std::uint64_t m_id;
    std::uint64_t m_cellId;
    std::string m_name;
};
