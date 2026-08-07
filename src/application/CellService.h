#pragma once

#include "domain/FinancialCell.h"
#include "domain/CellMemberSummary.h"
#include "domain/CellRole.h"
#include "CellRepository.h"
#include "UserRepository.h"
#include <optional>
#include <vector>

/** @brief Detailed outcome for an authorized cell mutation. */
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
    LAST_MANAGER_REQUIRED,
    STORAGE_ERROR
};

/** @brief Coordinates cell validation, membership, and authorization rules. */
class CellService
{
public:
    /** @brief Minimum accepted cell-name length. */
    static constexpr std::size_t MIN_CELL_NAME_LENGTH = 3;
    /** @brief Maximum accepted cell-name length. */
    static constexpr std::size_t MAX_CELL_NAME_LENGTH = 50;
    /** @brief Minimum accepted description length. */
    static constexpr std::size_t MIN_DESCRIPTION_LENGTH = 0;
    /** @brief Maximum accepted description length. */
    static constexpr std::size_t MAX_DESCRIPTION_LENGTH = 200;
    /** @brief Creates the service. @param cellRepository Cell persistence. @param userRepository User persistence. */
    CellService(CellRepository& cellRepository, UserRepository& userRepository);

    /** @brief Creates a cell. @param cellName Cell name. @param creatorId Creator ID. @param cellDescription Description. @return True on success. */
    bool createCell(const std::string& cellName, uint64_t creatorId, const std::string& cellDescription);
    /** @brief Adds a member. @param actingUserId Actor ID. @param cellId Cell ID. @param newUserId New member ID. @param role New member role. @return Operation result. */
    CellOperationResult addMemberToCell(uint64_t actingUserId, uint64_t cellId, uint64_t newUserId, CellRole role);
    /** @brief Changes a member role. @param actingUserId Actor ID. @param cellId Cell ID. @param memberUserId Member ID. @param role New role. @return Operation result. */
    CellOperationResult updateMemberRole(uint64_t actingUserId, uint64_t cellId, uint64_t memberUserId, CellRole role);
    /** @brief Removes a member. @param actingUserId Actor ID. @param cellId Cell ID. @param memberUserId Member ID. @return Operation result. */
    CellOperationResult removeMemberFromCell(uint64_t actingUserId, uint64_t cellId, uint64_t memberUserId);
    /** @brief Updates cell details. @param actingUserId Actor ID. @param cellId Cell ID. @param name New name. @param description New description. @return Operation result. */
    CellOperationResult updateCell(uint64_t actingUserId, uint64_t cellId, const std::string& name, const std::string& description);
    /** @brief Deletes a cell. @param actingUserId Actor ID. @param cellId Cell ID. @return Operation result. */
    CellOperationResult deleteCell(uint64_t actingUserId, uint64_t cellId);

    /** @brief Checks whether a cell exists. @param cellId Cell ID. @return True when found. */
    bool cellExists(uint64_t cellId) const;
    /** @brief Returns every cell. @return All cells. */
    std::vector<FinancialCell> getCells() const;
    /** @brief Returns cells accessible to a user. @param userId User ID. @return Accessible cells. */
    std::vector<FinancialCell> getCellsForUser(uint64_t userId) const;
    /** @brief Returns an accessible cell. @param actingUserId Actor ID. @param cellId Cell ID. @return Cell, or empty when inaccessible. */
    std::optional<FinancialCell> getCellForUser(uint64_t actingUserId, uint64_t cellId) const;
    /** @brief Returns cell members. @param actingUserId Actor ID. @param cellId Cell ID. @return Members, or an empty list when inaccessible. */
    std::vector<CellMember> getCellMembers(uint64_t actingUserId, uint64_t cellId) const;
    /** @brief Returns public member identities and roles. @param actingUserId Actor ID. @param cellId Cell ID. @return Member summaries, or empty optional when inaccessible. */
    std::optional<std::vector<CellMemberSummary>> getCellMemberSummaries(
        std::uint64_t actingUserId,
        std::uint64_t cellId) const;
    /** @brief Checks whether a user manages a cell. @param userId User ID. @param cellId Cell ID. @return True for a manager. */
    bool isManager(std::uint64_t userId, std::uint64_t cellId) const;

    /** @brief Validates a cell name. @param cellName Cell name. @return True when valid. */
    bool isCellNameValid(const std::string& cellName) const;
    /** @brief Validates a description. @param description Description. @return True when valid. */
    bool isDescriptionValid(const std::string& description) const;

private:
    /** @brief Counts managers assigned to a cell. @param cellId Cell ID. @return Manager count. */
    std::size_t managerCount(std::uint64_t cellId) const;
    /** @brief Checks cell membership. @param userId User ID. @param cellId Cell ID. @return True for a member. */
    bool isMember(uint64_t userId, uint64_t cellId) const;
    CellRepository& m_cellRepository;
    UserRepository& m_userRepository;
};
