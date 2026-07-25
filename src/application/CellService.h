#pragma once

#include "domain/FinancialCell.h"
#include "domain/CellRole.h"
#include "CellRepository.h"
#include <vector>

class CellService
{
public:
    static constexpr std::size_t MIN_CELL_NAME_LENGTH = 3;
    static constexpr std::size_t MAX_CELL_NAME_LENGTH = 50;
    static constexpr std::size_t MIN_DESCRIPTION_LENGTH = 0;
    static constexpr std::size_t MAX_DESCRIPTION_LENGTH = 200;
    explicit CellService(CellRepository& cellRepository);

    bool createCell(const std::string& cellName, uint64_t ownerId, const std::string& cellDescription);
    bool addMemberToCell(uint64_t actingUserId, uint64_t cellId, uint64_t newUserId, CellRole role);

    bool cellExists(uint64_t cellId) const;
    std::vector<FinancialCell> getCells() const;

private:
    bool isCellNameValid(const std::string& cellName) const;
    bool isDescriptionValid(const std::string& description) const;
    CellRepository& m_cellRepository;
};