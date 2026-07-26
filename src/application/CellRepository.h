#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "domain/CellMember.h"
#include "domain/FinancialCell.h"

/** @brief Persistence contract for cells and their memberships. */
class CellRepository
{
public:
    /** @brief Destroys the repository interface. */
    virtual ~CellRepository() = default;

    /** @brief Inserts a cell. @param cell Cell to persist. @return Persisted cell with its generated ID. */
    virtual std::optional<FinancialCell> insertCell(const FinancialCell& cell) = 0;
    /** @brief Finds a cell by ID. @param cellId Cell ID. @return Matching cell, or empty. */
    virtual std::optional<FinancialCell> findCellById(std::uint64_t cellId) const = 0;
    /** @brief Updates a cell. @param cell Updated cell data. @return True when updated. */
    virtual bool updateCell(const FinancialCell& cell) = 0;
    /** @brief Deletes a cell. @param cellId Cell ID. @return True when deleted. */
    virtual bool deleteCell(std::uint64_t cellId) = 0;
    /** @brief Finds cells owned by a user. @param ownerId Owner ID. @return Owned cells. */
    virtual std::vector<FinancialCell> findCellsByOwnerId(
        std::uint64_t ownerId) const = 0;
    /** @brief Returns every cell. @return All stored cells. */
    virtual std::vector<FinancialCell> findAllCells() const = 0;
    /** @brief Inserts a membership. @param member Membership data. @return True when inserted. */
    virtual bool insertMember(const CellMember& member) = 0;
    /** @brief Finds a membership. @param cellId Cell ID. @param userId User ID. @return Matching membership, or empty. */
    virtual std::optional<CellMember> findMember(
        std::uint64_t cellId,
        std::uint64_t userId) const = 0;
    /** @brief Finds members of a cell. @param cellId Cell ID. @return Cell memberships. */
    virtual std::vector<CellMember> findMembersByCellId(
        std::uint64_t cellId) const = 0;
    /** @brief Finds cells accessible to a user. @param userId User ID. @return Accessible cells. */
    virtual std::vector<FinancialCell> findCellsByUserId(
        std::uint64_t userId) const = 0;
    /** @brief Updates a member role. @param cellId Cell ID. @param userId User ID. @param role New role. @return True when updated. */
    virtual bool updateMemberRole(
        std::uint64_t cellId,
        std::uint64_t userId,
        CellRole role) = 0;
    /** @brief Deletes a membership. @param cellId Cell ID. @param userId User ID. @return True when deleted. */
    virtual bool deleteMember(
        std::uint64_t cellId,
        std::uint64_t userId) = 0;
};
