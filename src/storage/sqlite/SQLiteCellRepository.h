#pragma once

#include "application/CellRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"

/** @brief SQLite implementation of cell and membership persistence. */
class SQLiteCellRepository final : public CellRepository
{
public:
    /** @brief Creates the repository. @param database Shared database connection. */
    explicit SQLiteCellRepository(SQLiteDatabase& database);

    /** @copydoc CellRepository::insertCell */
    std::optional<FinancialCell> insertCell(
        const FinancialCell& cell) override;
    /** @copydoc CellRepository::findCellById */
    std::optional<FinancialCell> findCellById(
        std::uint64_t cellId) const override;
    /** @copydoc CellRepository::updateCell */
    bool updateCell(const FinancialCell& cell) override;
    /** @copydoc CellRepository::deleteCell */
    bool deleteCell(std::uint64_t cellId) override;
    /** @copydoc CellRepository::findCellsByCreatorId */
    std::vector<FinancialCell> findCellsByCreatorId(
        std::uint64_t creatorId) const override;
    /** @copydoc CellRepository::findAllCells */
    std::vector<FinancialCell> findAllCells() const override;
    /** @copydoc CellRepository::insertMember */
    bool insertMember(const CellMember& member) override;
    /** @copydoc CellRepository::findMember */
    std::optional<CellMember> findMember(
        std::uint64_t cellId,
        std::uint64_t userId) const override;
    /** @copydoc CellRepository::findMembersByCellId */
    std::vector<CellMember> findMembersByCellId(
        std::uint64_t cellId) const override;
    /** @copydoc CellRepository::findCellsByUserId */
    std::vector<FinancialCell> findCellsByUserId(
        std::uint64_t userId) const override;
    /** @copydoc CellRepository::updateMemberRole */
    bool updateMemberRole(
        std::uint64_t cellId,
        std::uint64_t userId,
        CellRole role) override;
    /** @copydoc CellRepository::deleteMember */
    bool deleteMember(
        std::uint64_t cellId,
        std::uint64_t userId) override;

private:
    SQLiteDatabase& m_database;
};
