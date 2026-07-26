#pragma once

#include "application/CellRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"

class SQLiteCellRepository final : public CellRepository
{
public:
    explicit SQLiteCellRepository(SQLiteDatabase& database);

    std::optional<FinancialCell> insertCell(
        const FinancialCell& cell) override;
    std::optional<FinancialCell> findCellById(
        std::uint64_t cellId) const override;
    std::vector<FinancialCell> findCellsByOwnerId(
        std::uint64_t ownerId) const override;
    std::vector<FinancialCell> findAllCells() const override;
    bool insertMember(const CellMember& member) override;
    std::optional<CellMember> findMember(
        std::uint64_t cellId,
        std::uint64_t userId) const override;
    std::vector<CellMember> findMembersByCellId(
        std::uint64_t cellId) const override;
    std::vector<FinancialCell> findCellsByUserId(
        std::uint64_t userId) const override;
    bool updateMemberRole(
        std::uint64_t cellId,
        std::uint64_t userId,
        CellRole role) override;
    bool deleteMember(
        std::uint64_t cellId,
        std::uint64_t userId) override;

private:
    SQLiteDatabase& m_database;
};
