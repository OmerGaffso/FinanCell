#include <stdexcept>
#include "CellService.h"
#include "utils/StringUtils.h"

CellService::CellService(CellRepository& cellRepository, UserRepository& userRepository)
    : m_cellRepository(cellRepository), m_userRepository(userRepository)
{
}

bool CellService::createCell(const std::string& cellName, uint64_t ownerId, const std::string& cellDescription)
{
    const std::string trimmedCellName = StringUtils::trim(cellName);
    const std::string trimmedCellDescription = StringUtils::trim(cellDescription);

    if (ownerId == 0)
    {
        throw std::invalid_argument("A cell must have a valid owner.");
    }

    if (!isCellNameValid(trimmedCellName))
    {
        throw std::invalid_argument("Cell name must be between " +
                                    std::to_string(MIN_CELL_NAME_LENGTH) + " and " +
                                    std::to_string(MAX_CELL_NAME_LENGTH) + " characters.");
    }

    if (!isDescriptionValid(trimmedCellDescription))
    {
        throw std::invalid_argument("Cell description must be between " +
                                    std::to_string(MIN_DESCRIPTION_LENGTH) + " and " +
                                    std::to_string(MAX_DESCRIPTION_LENGTH) + " characters.");
    }

    return m_cellRepository
        .insertCell(FinancialCell(
            0,
            trimmedCellName,
            trimmedCellDescription,
            "ILS",
            ownerId))
        .has_value();
}

CellOperationResult CellService::addMemberToCell(uint64_t actingUserId, uint64_t cellId, uint64_t newUserId, CellRole role)
{
    const auto cell = m_cellRepository.findCellById(cellId);
    if (!cell) return CellOperationResult::CELL_NOT_FOUND;
    if (!isOwner(actingUserId, *cell)) return CellOperationResult::NOT_AUTHORIZED;
    if (role == CellRole::OWNER) return CellOperationResult::INVALID_ROLE;
    if (!m_userRepository.findUserById(newUserId)) return CellOperationResult::USER_NOT_FOUND;
    if (m_cellRepository.findMember(cellId, newUserId)) return CellOperationResult::ALREADY_MEMBER;
    return m_cellRepository.insertMember({newUserId, cellId, role})
        ? CellOperationResult::SUCCESS : CellOperationResult::STORAGE_ERROR;
}

CellOperationResult CellService::updateMemberRole(
    uint64_t actingUserId,
    uint64_t cellId,
    uint64_t memberUserId,
    CellRole role)
{
    const auto cell = m_cellRepository.findCellById(cellId);
    if (!cell) return CellOperationResult::CELL_NOT_FOUND;
    if (!isOwner(actingUserId, *cell)) return CellOperationResult::NOT_AUTHORIZED;
    if (memberUserId == cell->getOwnerId()) return CellOperationResult::CANNOT_MODIFY_OWNER;
    if (role == CellRole::OWNER) return CellOperationResult::INVALID_ROLE;
    if (!m_cellRepository.findMember(cellId, memberUserId)) return CellOperationResult::MEMBER_NOT_FOUND;
    return m_cellRepository.updateMemberRole(cellId, memberUserId, role)
        ? CellOperationResult::SUCCESS : CellOperationResult::STORAGE_ERROR;
}

CellOperationResult CellService::removeMemberFromCell(
    uint64_t actingUserId,
    uint64_t cellId,
    uint64_t memberUserId)
{
    const auto cell = m_cellRepository.findCellById(cellId);
    if (!cell) return CellOperationResult::CELL_NOT_FOUND;
    if (!isOwner(actingUserId, *cell)) return CellOperationResult::NOT_AUTHORIZED;
    if (memberUserId == cell->getOwnerId()) return CellOperationResult::CANNOT_MODIFY_OWNER;
    if (!m_cellRepository.findMember(cellId, memberUserId)) return CellOperationResult::MEMBER_NOT_FOUND;
    return m_cellRepository.deleteMember(cellId, memberUserId)
        ? CellOperationResult::SUCCESS : CellOperationResult::STORAGE_ERROR;
}

CellOperationResult CellService::updateCell(
    uint64_t actingUserId,
    uint64_t cellId,
    const std::string& name,
    const std::string& description)
{
    const auto cell = m_cellRepository.findCellById(cellId);
    const std::string trimmedName = StringUtils::trim(name);
    const std::string trimmedDescription = StringUtils::trim(description);
    if (!cell) return CellOperationResult::CELL_NOT_FOUND;
    if (!isOwner(actingUserId, *cell)) return CellOperationResult::NOT_AUTHORIZED;
    if (!isCellNameValid(trimmedName) || !isDescriptionValid(trimmedDescription))
        return CellOperationResult::INVALID_INPUT;
    return m_cellRepository.updateCell(FinancialCell(
        cellId,
        trimmedName,
        trimmedDescription,
        cell->getCurrency(),
        cell->getOwnerId())) ? CellOperationResult::SUCCESS : CellOperationResult::STORAGE_ERROR;
}

CellOperationResult CellService::deleteCell(uint64_t actingUserId, uint64_t cellId)
{
    const auto cell = m_cellRepository.findCellById(cellId);
    if (!cell) return CellOperationResult::CELL_NOT_FOUND;
    if (!isOwner(actingUserId, *cell)) return CellOperationResult::NOT_AUTHORIZED;
    return m_cellRepository.deleteCell(cellId)
        ? CellOperationResult::SUCCESS : CellOperationResult::STORAGE_ERROR;
}

bool CellService::cellExists(uint64_t cellId) const
{
    return m_cellRepository.findCellById(cellId).has_value();
}

std::vector<FinancialCell> CellService::getCells() const
{
    return m_cellRepository.findAllCells();
}

std::vector<FinancialCell> CellService::getCellsForUser(uint64_t userId) const
{
    return m_cellRepository.findCellsByUserId(userId);
}

std::optional<FinancialCell> CellService::getCellForUser(
    uint64_t actingUserId,
    uint64_t cellId) const
{
    if (!isMember(actingUserId, cellId))
    {
        return std::nullopt;
    }
    return m_cellRepository.findCellById(cellId);
}

std::vector<CellMember> CellService::getCellMembers(
    uint64_t actingUserId,
    uint64_t cellId) const
{
    if (!isMember(actingUserId, cellId))
    {
        return {};
    }
    return m_cellRepository.findMembersByCellId(cellId);
}

std::optional<std::vector<CellMemberSummary>> CellService::getCellMemberSummaries(
    std::uint64_t actingUserId,
    std::uint64_t cellId) const
{
    if (!isMember(actingUserId, cellId)) return std::nullopt;

    std::vector<CellMemberSummary> summaries;
    for (const CellMember& member : m_cellRepository.findMembersByCellId(cellId))
    {
        const auto user = m_userRepository.findUserSummaryById(member.userId);
        if (!user)
            throw std::runtime_error("A cell membership references an unknown user.");
        summaries.emplace_back(
            user->getUserId(),
            user->getUsername(),
            user->getDisplayName(),
            member.role);
    }
    return summaries;
}

bool CellService::isOwner(uint64_t userId, const FinancialCell& cell) const
{
    return userId != 0 && cell.getOwnerId() == userId;
}

bool CellService::isMember(uint64_t userId, uint64_t cellId) const
{
    return userId != 0 && m_cellRepository.findMember(cellId, userId).has_value();
}

bool CellService::isCellNameValid(const std::string& cellName) const
{
    const std::size_t length = StringUtils::trim(cellName).length();
    if (length < MIN_CELL_NAME_LENGTH || length > MAX_CELL_NAME_LENGTH)
    {
        return false;
    }

    return true;
}

bool CellService::isDescriptionValid(const std::string& description) const
{
    const std::size_t length = StringUtils::trim(description).length();
    if (length < MIN_DESCRIPTION_LENGTH || length > MAX_DESCRIPTION_LENGTH)
    {
        return false;
    }

    return true;
}
