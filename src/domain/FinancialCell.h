#pragma once

#include <cstdint>
#include <string>

class FinancialCell
{
public:
    FinancialCell(uint64_t id, std::string name, uint64_t creatorUserId);
    ~FinancialCell() = default;

    uint64_t getCellId() const;
    uint64_t getCreatorId() const;
    const std::string& getCellName() const;

private:
    uint64_t m_creatorId;
    uint64_t m_cellId;
    std::string m_cellName;
}; 