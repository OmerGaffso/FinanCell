#pragma once

#include <optional>
#include <string>
#include <vector>

#include "domain/FinancialCell.h"

class CellRepository
{
public:
    virtual ~CellRepository() = default;

    virtual bool insertCell(const FinancialCell& cell) = 0;
    virtual std::optional<FinancialCell> findCellById(uint64_t cellId) const = 0;
    virtual std::vector<FinancialCell> findAllCells() const = 0;
};
