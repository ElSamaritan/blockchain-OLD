#pragma once

#include <cinttypes>
#include <chrono>

#include <Xi/Utils/Conversion.h>

#include "config/_Impl/BeginDifficulty.h"

/*!
 * Configuring LWMA
 *
 * _Index     : The order in which the different configurations apply.
 * _Height    : When shall this algorithm be applied.
 * _Window    : The amount of previous blocks that shall be considered for calculating the new difficulty.
 * _Initial   : The initial difficulty chosen if not enough block have been mined to obtain all data required.
 * _TimeLimit : The maximum amount of time a new block can differ from it predecessor in order to be accepted.
 * _Algirthm  : The implementation to use for difficulty calculation from that checkpoint on to the next, if present
 *
 */

// clang-format off
//                      (_Index, _Height, _Window, _Initial, _TimeLimit,                           _Algorithm)
MakeDifficultyCheckpoint(    0,      0,      64,   1000,       2_h,    ::CryptoNote::Difficulty::LWMA_2)
// clang-format on

#define CURRENT_DIFFICULTY_CHECKPOINT_INDEX 0

#include "config/_Impl/EndDifficulty.h"
