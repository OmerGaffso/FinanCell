#pragma once

#include <cstdint>
#include <string>

/** @brief Shared financial workspace created by a user and managed through memberships. */
class FinancialCell
{
public:
    /** @brief Constructs a cell. @param id Cell ID. @param name Name. @param description Description. @param currency ISO currency code. @param creatorId Original creator ID. */
    FinancialCell(uint64_t id, std::string name, std::string description, std::string currency, uint64_t creatorId);
    /** @brief Returns the cell ID. @return Cell ID. */
    uint64_t getCellId() const;
    /** @brief Returns the original creator ID. @return Creator user ID. */
    uint64_t getCreatorId() const;
    /** @brief Returns the name. @return Cell name. */
    const std::string& getCellName() const;
    /** @brief Returns the description. @return Cell description. */
    const std::string& getCellDescription() const;
    /** @brief Returns the currency. @return ISO currency code. */
    const std::string& getCurrency() const;

private:
    uint64_t m_cellId;
    uint64_t m_creatorId;
    std::string m_cellName;
    std::string m_cellDescription;
    std::string m_currency;
};
