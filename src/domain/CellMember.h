#pragma once

#include <cstdint>
#include "CellRole.h"

/** Associates a user with a cell and an authorization role. */
struct CellMember
{
    uint64_t userId;
    uint64_t cellId;
    CellRole role;
};
