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

private:
    SQLiteDatabase& m_database;
};
