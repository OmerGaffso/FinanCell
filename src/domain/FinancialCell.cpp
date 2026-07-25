#include "FinancialCell.h"

FinancialCell::FinancialCell(uint64_t id, std::string name, std::string description, std::string usesCurrency, uint64_t ownerId)
    : m_cellId(id), m_cellName(name), m_cellDescription(description), m_usesCurrency(usesCurrency), m_ownerId(ownerId)
{

}

FinancialCell::~FinancialCell()
{

}

uint64_t FinancialCell::getCellId() const
{
    return m_cellId;
}

uint64_t FinancialCell::getOwnerId() const
{
    return m_ownerId;
}

const std::string& FinancialCell::getCellName() const
{
    return m_cellName;
}

const std::string& FinancialCell::getCellDescription() const
{
    return m_cellDescription;
}

const std::string& FinancialCell::getUsesCurrency() const
{
    return m_usesCurrency;
}
