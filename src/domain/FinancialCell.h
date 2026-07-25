#pragma once

#include <cstdint>
#include <string>

class FinancialCell
{
public:
    FinancialCell(uint64_t id, std::string name, std::string description, std::string usesCurrency, uint64_t ownerId);
    ~FinancialCell() = default;

    uint64_t getCellId() const;
    uint64_t getOwnerId() const;
    const std::string& getCellName() const;
    const std::string& getCellDescription() const;
    const std::string& getUsesCurrency() const;

private:
    uint64_t m_ownerId;
    uint64_t m_cellId;
    std::string m_cellName;
    std::string m_cellDescription;
    std::string m_usesCurrency;
}; 