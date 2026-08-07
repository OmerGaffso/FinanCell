#include "FinancialCell.h"

#include <utility>

FinancialCell::FinancialCell(uint64_t id, std::string name, std::string description, std::string currency, uint64_t creatorId)
    : m_cellId(id),
      m_creatorId(creatorId),
      m_cellName(std::move(name)),
      m_cellDescription(std::move(description)),
      m_currency(std::move(currency))
{
}

uint64_t FinancialCell::getCellId() const
{
    return m_cellId;
}

uint64_t FinancialCell::getCreatorId() const
{
    return m_creatorId;
}

const std::string& FinancialCell::getCellName() const
{
    return m_cellName;
}

const std::string& FinancialCell::getCellDescription() const
{
    return m_cellDescription;
}

const std::string& FinancialCell::getCurrency() const
{
    return m_currency;
}
