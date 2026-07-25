#pragma once

#include "/domain/FinancialCell.h"

class CellService
{
public:
    bool createCell(const std::string& cellName, uint64_t ownerId, const std::string& cellDescription, const std::string& usesCurrency);
    bool isCellNameLengthValid(const std::string& cellName) const;
    bool isCellDescriptionLengthValid(const std::string& cellDescription) const;
    bool isUsesCurrencyLengthValid(const std::string& usesCurrency) const;
    bool cellExists(const std::string& cellName) const;
    bool isValidCurrencyCode(const std::string& currencyCode) const;
    bool isValidCurrencySymbol(const std::string& currencySymbol) const;
    bool isValidCurrencyName(const std::string& currencyName) const;
    bool addMemberToCell(uint64_t actingUserId, uint64_t cellId, uint64_t newUserId, CellRole role);
};