#pragma once

#include <CryptoNoteCore/HashAlgorithms.h>

#include "config/_Impl/BeginHashes.h"

// clang-format off
//                (_Index, _Version,                            _Algorithm)
MakeHashCheckpoint(     0,        1,  ::CryptoNote::Hashes::InitBlockchain)
MakeHashCheckpoint(     1,        2,  ::CryptoNote::Hashes::CNX_v0        )
// clang-format on

#define CURRENT_HASH_CHECKPOINT_INDEX 1

#include "config/_Impl/EndHashes.h"
