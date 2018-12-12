#pragma once

#include "config/_Impl/BeginDust.h"

// clang-format off
//                (_Index, _Version,   _Dust)
MakeDustCheckpoint(     0,        1,     100)
MakeDustCheckpoint(     1,        5,    1000)
MakeDustCheckpoint(     2,        6,   10000)

#define CURRENT_DUST_CHECKPOINT_INDEX 2

//                      (_Index, _Version,   _Dust)
MakeFusionDustCheckpoint(     0,        1,      10)
MakeFusionDustCheckpoint(     1,        6,     200)
// clang-format on

#define CURRENT_FUSION_DUST_CHECKPOINT_INDEX 1

#include "config/_Impl/EndDust.h"
