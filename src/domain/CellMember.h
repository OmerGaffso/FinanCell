#pragma once

#include <cstdint>
#include "CellRole.h"

/** Associates a user with a cell and an authorization role. */
struct CellMember
{
    /** @brief Member user ID. */
    uint64_t userId;
    /** @brief Cell ID. */
    uint64_t cellId;
    /** @brief User's role in the cell. */
    CellRole role;
};
