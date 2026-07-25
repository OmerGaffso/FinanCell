#include "FinancialCell.h"

FinancialCell::FinancialCell(uint64_t id, std::string name, uint64_t creatorUserId)
    : m_cellId(id), m_cellName(name), m_creatorId(creatorUserId)
    {}

FinancialCell::~FinancialCell()
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
