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

bool CellService::addMemberToCell(uint64_t actingUserId, uint64_t cellId, uint64_t newUserId, CellRole role)
{
    const auto cell = m_cellRepository.findCellById(cellId);
    if (!cell || !isOwner(actingUserId, *cell) || role == CellRole::OWNER ||
        !m_userRepository.findUserById(newUserId) ||
        m_cellRepository.findMember(cellId, newUserId))
    {
        return false;
    }

    return m_cellRepository.insertMember({newUserId, cellId, role});
}

bool CellService::updateMemberRole(
    uint64_t actingUserId,
    uint64_t cellId,
    uint64_t memberUserId,
    CellRole role)
{
    const auto cell = m_cellRepository.findCellById(cellId);
    if (!cell || !isOwner(actingUserId, *cell) ||
        memberUserId == cell->getOwnerId() || role == CellRole::OWNER ||
        !m_cellRepository.findMember(cellId, memberUserId))
    {
        return false;
    }

    return m_cellRepository.updateMemberRole(cellId, memberUserId, role);
}

bool CellService::removeMemberFromCell(
    uint64_t actingUserId,
    uint64_t cellId,
    uint64_t memberUserId)
{
    const auto cell = m_cellRepository.findCellById(cellId);
    if (!cell || !isOwner(actingUserId, *cell) ||
        memberUserId == cell->getOwnerId())
    {
        return false;
    }

    return m_cellRepository.deleteMember(cellId, memberUserId);
}

bool CellService::updateCell(
    uint64_t actingUserId,
    uint64_t cellId,
    const std::string& name,
    const std::string& description)
{
    const auto cell = m_cellRepository.findCellById(cellId);
    const std::string trimmedName = StringUtils::trim(name);
    const std::string trimmedDescription = StringUtils::trim(description);
    if (!cell || !isOwner(actingUserId, *cell) ||
        !isCellNameValid(trimmedName) || !isDescriptionValid(trimmedDescription))
    {
        return false;
    }

    return m_cellRepository.updateCell(FinancialCell(
        cellId,
        trimmedName,
        trimmedDescription,
        cell->getUsesCurrency(),
        cell->getOwnerId()));
}

bool CellService::deleteCell(uint64_t actingUserId, uint64_t cellId)
{
    const auto cell = m_cellRepository.findCellById(cellId);
    return cell && isOwner(actingUserId, *cell) && m_cellRepository.deleteCell(cellId);
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
