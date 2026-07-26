#pragma once

#include "domain/FinancialCell.h"
#include "domain/CellRole.h"
#include "CellRepository.h"
#include "UserRepository.h"
#include <optional>
#include <vector>

/** Detailed outcome for an authorized cell mutation. */
enum class CellOperationResult
{
    SUCCESS,
    CELL_NOT_FOUND,
    USER_NOT_FOUND,
    ALREADY_MEMBER,
    MEMBER_NOT_FOUND,
    INVALID_ROLE,
    INVALID_INPUT,
    NOT_AUTHORIZED,
    CANNOT_MODIFY_OWNER,
    STORAGE_ERROR
};

/** Coordinates cell validation, membership, and authorization rules. */
class CellService
{
public:
    static constexpr std::size_t MIN_CELL_NAME_LENGTH = 3;
    static constexpr std::size_t MAX_CELL_NAME_LENGTH = 50;
    static constexpr std::size_t MIN_DESCRIPTION_LENGTH = 0;
    static constexpr std::size_t MAX_DESCRIPTION_LENGTH = 200;
    CellService(CellRepository& cellRepository, UserRepository& userRepository);

    bool createCell(const std::string& cellName, uint64_t ownerId, const std::string& cellDescription);
    CellOperationResult addMemberToCell(uint64_t actingUserId, uint64_t cellId, uint64_t newUserId, CellRole role);
    CellOperationResult updateMemberRole(uint64_t actingUserId, uint64_t cellId, uint64_t memberUserId, CellRole role);
    CellOperationResult removeMemberFromCell(uint64_t actingUserId, uint64_t cellId, uint64_t memberUserId);
    CellOperationResult updateCell(uint64_t actingUserId, uint64_t cellId, const std::string& name, const std::string& description);
    CellOperationResult deleteCell(uint64_t actingUserId, uint64_t cellId);

    bool cellExists(uint64_t cellId) const;
    std::vector<FinancialCell> getCells() const;
    std::vector<FinancialCell> getCellsForUser(uint64_t userId) const;
    std::optional<FinancialCell> getCellForUser(uint64_t actingUserId, uint64_t cellId) const;
    std::vector<CellMember> getCellMembers(uint64_t actingUserId, uint64_t cellId) const;

    bool isCellNameValid(const std::string& cellName) const;
    bool isDescriptionValid(const std::string& description) const;

private:
    bool isOwner(uint64_t userId, const FinancialCell& cell) const;
    bool isMember(uint64_t userId, uint64_t cellId) const;
    CellRepository& m_cellRepository;
    UserRepository& m_userRepository;
};
