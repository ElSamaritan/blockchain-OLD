#pragma once

#include "config/_Impl/BeginMixinConfig.h"

/*!
 * Creating a new Mixin Checkpoint
 *
 * Copy one of the MakeCheckpoint lines below to the bottom of the listing. Adjust the index to the previous index
 * plus one. Finally increment the CURRENT_MIXIN_CHECKPOINT_INDEX definition below the list to the newly introduced
 * index.
 *
 * There are no further changes to the code required. If you changed the code for a running blockchain you may want
 * to add the choosen height to the forks array to notify running daemons about the upcomming fork.
 *
 */

// clang-format off
//                 (_Index, _Version, _Min, _Max, _Default)
MakeMixinCheckpoint(     0,        1,    0,    4,        0)
MakeMixinCheckpoint(     1,        5,    4,   10,        4)
MakeMixinCheckpoint(     2,        6,    4,   20,        8)
MakeMixinCheckpoint(     3,        7,   10,   30,       20)
// clang-format on

#define CURRENT_MIXIN_CHECKPOINT_INDEX 3

#include "config/_Impl/EndMixinConfig.h"
