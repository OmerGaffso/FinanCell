#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "domain/FinancialCell.h"

class CellRepository
{
public:
    virtual ~CellRepository() = default;

    virtual std::optional<FinancialCell> insertCell(const FinancialCell& cell) = 0;
    virtual std::optional<FinancialCell> findCellById(std::uint64_t cellId) const = 0;
    virtual std::vector<FinancialCell> findCellsByOwnerId(
        std::uint64_t ownerId) const = 0;
};
