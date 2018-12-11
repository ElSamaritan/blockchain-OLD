#pragma once

#include "CryptoNoteCore/CachedBlock.h"

namespace CryptoNote {
namespace Hashes {
struct InitBlockchain {
  void operator()(const CachedBlock& block, Crypto::Hash& hash) const;
};

struct CNX_v0 {
  void operator()(const CachedBlock& block, Crypto::Hash& hash) const;
};
}  // namespace Hashes
}  // namespace CryptoNote
