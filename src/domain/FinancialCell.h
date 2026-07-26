#pragma once

#include <cstdint>
#include <string>

/** @brief Shared financial workspace owned by one user. */
class FinancialCell
{
public:
    /** @brief Constructs a cell. @param id Cell ID. @param name Name. @param description Description. @param currency ISO currency code. @param ownerId Owner ID. */
    FinancialCell(uint64_t id, std::string name, std::string description, std::string currency, uint64_t ownerId);
    /** @brief Returns the cell ID. @return Cell ID. */
    uint64_t getCellId() const;
    /** @brief Returns the owner ID. @return Owner user ID. */
    uint64_t getOwnerId() const;
    /** @brief Returns the name. @return Cell name. */
    const std::string& getCellName() const;
    /** @brief Returns the description. @return Cell description. */
    const std::string& getCellDescription() const;
    /** @brief Returns the currency. @return ISO currency code. */
    const std::string& getCurrency() const;

private:
    uint64_t m_cellId;
    uint64_t m_ownerId;
    std::string m_cellName;
    std::string m_cellDescription;
    std::string m_currency;
};
