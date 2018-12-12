#include "CryptoNoteCore/HashAlgorithms.h"

#include "crypto/cnx/cnx.h"

void CryptoNote::Hashes::InitBlockchain::operator()(const CryptoNote::CachedBlock &block, Crypto::Hash &hash) const {
  const auto &rawHashingBlock = block.getBlockHashingBinaryArray();
  cn_slow_hash_v0(rawHashingBlock.data(), rawHashingBlock.size(), hash);
}

void CryptoNote::Hashes::CNX_v0::operator()(const CryptoNote::CachedBlock &block, Crypto::Hash &hash) const {
  const auto &rawHashingBlock = block.getParentBlockHashingBinaryArray(true);
  Crypto::CNX::Hash_v0{}(rawHashingBlock.data(), rawHashingBlock.size(), hash, block.getBlockIndex());
}
