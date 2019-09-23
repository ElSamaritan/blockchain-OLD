/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

#include "Xi/Mnemonic/Mnemonic.hpp"

#include <cinttypes>
#include <optional>
#include <cstring>
#include <cassert>
#include <algorithm>

#include <Xi/Algorithm/Math.h>
#include <Xi/Algorithm/String.h>
#include <Xi/Exceptions.hpp>
#include <Xi/Crypto/Hash/Sha2.hh>

#include "Xi/Mnemonic/MnemonicError.hpp"

namespace {
#define XI_MNEMONIC_NTH_BIT(DATA, POS) static_cast<size_t>((DATA[POS / 8] >> (7 - (POS % 8))) & 0b1)

static size_t extractIndex(Xi::ConstByteSpan data, Xi::ConstByteSpan checksum, const size_t pos, const size_t count) {
  size_t reval = 0;
  size_t iBit = 0;
  for (iBit = pos; iBit < pos + count && iBit < data.size_bytes() * 8ULL; ++iBit) {
    reval = (reval << 1ULL) | XI_MNEMONIC_NTH_BIT(data.data(), iBit);
  }
  for (; iBit < pos + count; ++iBit) {
    const size_t bytePos = (iBit - data.size_bytes() * 8ULL) / 8ULL;
    const size_t byteBitIndex = (iBit - data.size_bytes() * 8ULL) % 8;
    reval = (reval << 1ULL) | static_cast<size_t>((checksum[bytePos] >> (7 - byteBitIndex)) & 0b1);
  }
  return reval;
}

static void emplaceIndex(Xi::ByteSpan data, size_t index, const size_t pos, const size_t count) {
  for (size_t i = 0; i < count; ++i) {
    const size_t iBit = pos + i;
    const size_t bytePos = iBit / 8ULL;
    if (!(bytePos < data.size())) {
      break;
    }
    data[bytePos] =
        static_cast<xi_byte_t>(data[bytePos] << 1) | static_cast<xi_byte_t>(((index >> (count - 1 - i)) & 0b1));
  }
}

struct Entropy {
  size_t SourceLength;
  size_t ChecksumLength;
  size_t MnemonicLength;
};

static const std::vector<Entropy> Entropies{{
    {128, 4, 12},
    {160, 5, 15},
    {192, 6, 18},
    {224, 7, 21},
    {256, 8, 24},
}};

static std::optional<Entropy> entropyBySourceLength(const size_t length) {
  auto search =
      std::find_if(begin(Entropies), end(Entropies), [length](const auto &e) { return e.SourceLength == length; });
  return search == end(Entropies) ? std::optional<Entropy>{} : std::make_optional(*search);
}
static std::optional<Entropy> entropyByMnemonicLength(const size_t length) {
  auto search =
      std::find_if(begin(Entropies), end(Entropies), [length](const auto &e) { return e.MnemonicLength == length; });
  return search == end(Entropies) ? std::optional<Entropy>{} : std::make_optional(*search);
}

}  // namespace

namespace Xi {
namespace Mnemonic {

using Checksum = Crypto::Hash::Sha2::Hash256;

Result<std::string> encode(ConstByteSpan data, const WordList &wordlist) {
  XI_ERROR_TRY
  const auto entropy = entropyBySourceLength(data.size_bytes() * 8ULL);
  XI_FAIL_IF_NOT(entropy.has_value(), MnemonicError::InvalidEntropy)
  const auto checksum = Checksum::compute(data);

  std::vector<std::string> reval{};
  reval.reserve(entropy->MnemonicLength);

  for (size_t bitOffset = 0; bitOffset < entropy->SourceLength + entropy->ChecksumLength;
       bitOffset += wordlist.bits()) {
    reval.emplace_back(wordlist[extractIndex(data, checksum.span(), bitOffset, wordlist.bits())]);
  }

  return success(join(reval, " "));
  XI_ERROR_CATCH
}

Result<ByteVector> decode(const std::string &mnemonic, const WordList &wordlist) {
  XI_ERROR_TRY
  const auto words = split(trim(toLower(mnemonic)), " ");

  const auto entropy = entropyByMnemonicLength(words.size());
  XI_FAIL_IF_NOT(entropy.has_value(), MnemonicError::InvalidEntropy)

  ByteVector reval{};
  reval.resize((entropy->SourceLength + entropy->ChecksumLength) / 8ULL, 0);

  size_t i = 0;
  for (const auto &word : words) {
    const auto index = wordlist[word];
    XI_ERROR_PROPAGATE(index)
    emplaceIndex(reval, *index, i, wordlist.bits());
    i += wordlist.bits();
  }

  ConstByteSpan data{reval.data(), entropy->SourceLength / 8ULL};
  const auto expectedChecksum = Checksum::compute(data);
  for (size_t iChecksumBit = 0; i < entropy->ChecksumLength; ++i) {
    const auto lhs = XI_MNEMONIC_NTH_BIT(reval.data(), entropy->SourceLength + iChecksumBit);
    const auto rhs = XI_MNEMONIC_NTH_BIT(expectedChecksum.data(), iChecksumBit);
    XI_FAIL_IF_NOT(lhs == rhs, MnemonicError::InvalidChecksum)
  }

  reval.resize(data.size_bytes());
  return success(move(reval));

  XI_ERROR_CATCH
}

Result<void> decode(const std::string &mnemonic, ByteSpan out, const WordList &wordlist) {
  XI_ERROR_TRY
  const auto inner = decode(mnemonic, wordlist).takeOrThrow();
  XI_FAIL_IF_NOT(inner.size() == out.size_bytes(), MnemonicError::UnexpectedSize)
  std::memcpy(out.data(), inner.data(), out.size_bytes());
  return success();
  XI_ERROR_CATCH
}

}  // namespace Mnemonic
}  // namespace Xi
