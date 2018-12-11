#pragma once

#include "config/_Impl/BeginDust.h"

// clang-format off
//                (_Index, _Version,   _Dust)
MakeDustCheckpoint(    0,       1,    100)
MakeDustCheckpoint(    1,       4,   1000)
MakeDustCheckpoint(    2,       5,  10000)
// clang-format on

#define CURRENT_DUST_CHECKPOINT_INDEX 2

// clang-format off
//                      (_Index, _Version,   _Dust)
MakeFusionDustCheckpoint(    0,       1,      0)
MakeFusionDustCheckpoint(    1,       5,    200)
// clang-format on

#define CURRENT_FUSION_DUST_CHECKPOINT_INDEX 1

#include "config/_Impl/EndDust.h"
