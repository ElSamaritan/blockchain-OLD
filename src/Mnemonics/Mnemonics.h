// Copyright 2014-2018 The Monero Developers
// Copyright 2018 The TurtleCoin Developers
// Copyright 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "CryptoNoteCore/CryptoNote.h"
#include <tuple>
#include <vector>
#include <cinttypes>

namespace Mnemonics {
std::tuple<std::string, Crypto::SecretKey> MnemonicToPrivateKey(const std::string words);

std::tuple<std::string, Crypto::SecretKey> MnemonicToPrivateKey(const std::vector<std::string> words);

std::string PrivateKeyToMnemonic(const Crypto::SecretKey privateKey);

bool HasValidChecksum(const std::vector<std::string> words);

std::string GetChecksumWord(const std::vector<std::string> words);

std::vector<uint32_t> GetWordIndexes(const std::vector<std::string> words);
}  // namespace Mnemonics
