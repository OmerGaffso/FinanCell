#pragma once

#include "domain/FinancialCell.h"
#include "domain/CellRole.h"

class CellService
{
public:
    bool createCell(const std::string& cellName, uint64_t ownerId, const std::string& cellDescription, const std::string& usesCurrency);
    bool addMemberToCell(uint64_t actingUserId, uint64_t cellId, uint64_t newUserId, CellRole role);
};