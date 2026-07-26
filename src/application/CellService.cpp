#include <stdexcept>
#include "CellService.h"
#include "utils/StringUtils.h"

CellService::CellService(CellRepository& cellRepository)
    : m_cellRepository(cellRepository)
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
    // Need to add Cell member table of connections by user id and cell id.
    return true;
}

bool CellService::cellExists(uint64_t cellId) const
{
    return m_cellRepository.findCellById(cellId).has_value();
}

std::vector<FinancialCell> CellService::getCells() const
{
    return m_cellRepository.findAllCells();
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
