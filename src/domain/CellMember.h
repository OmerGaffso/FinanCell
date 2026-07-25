#pragma once

#include <cstdint>
#include "CellRole.h"

struct CellMember
{
    uint64_t userId;
    uint64_t cellId;
    CellRole role;
};
