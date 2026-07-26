#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "domain/CellMember.h"
#include "domain/FinancialCell.h"

class CellRepository
{
public:
    virtual ~CellRepository() = default;

    virtual std::optional<FinancialCell> insertCell(const FinancialCell& cell) = 0;
    virtual std::optional<FinancialCell> findCellById(std::uint64_t cellId) const = 0;
    virtual bool updateCell(const FinancialCell& cell) = 0;
    virtual bool deleteCell(std::uint64_t cellId) = 0;
    virtual std::vector<FinancialCell> findCellsByOwnerId(
        std::uint64_t ownerId) const = 0;
    virtual std::vector<FinancialCell> findAllCells() const = 0;
    virtual bool insertMember(const CellMember& member) = 0;
    virtual std::optional<CellMember> findMember(
        std::uint64_t cellId,
        std::uint64_t userId) const = 0;
    virtual std::vector<CellMember> findMembersByCellId(
        std::uint64_t cellId) const = 0;
    virtual std::vector<FinancialCell> findCellsByUserId(
        std::uint64_t userId) const = 0;
    virtual bool updateMemberRole(
        std::uint64_t cellId,
        std::uint64_t userId,
        CellRole role) = 0;
    virtual bool deleteMember(
        std::uint64_t cellId,
        std::uint64_t userId) = 0;
};
