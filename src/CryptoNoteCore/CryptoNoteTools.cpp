// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#include "CryptoNoteTools.h"
#include "CryptoNoteFormatUtils.h"

#include <unordered_map>
#include <utility>
#include <limits>
#include <algorithm>
#include <iterator>

#include <Xi/Algorithm/Math.h>

using namespace CryptoNote;

namespace {
struct AmountDecompositionUnit {
  const uint64_t Amount;
  const uint8_t DecadeIndex;
  const uint8_t Digit;

  AmountDecompositionUnit(uint64_t amount, uint8_t decadeIndex, uint8_t digit)
      : Amount{amount}, DecadeIndex{decadeIndex}, Digit{digit} {
  }
};

class AmountDecompositions {
  std::unordered_map<uint64_t, AmountDecompositionUnit> m_units;
  AmountDecompositions() {
    for (uint8_t decade = 0; decade < std::numeric_limits<uint64_t>::digits10; ++decade) {
      uint64_t iDecadeUnit = Xi::pow64(10, decade);
      for (uint8_t digit = 1; digit < 10; ++digit) {
        uint64_t iAmount = iDecadeUnit * digit;
        m_units.emplace(std::piecewise_construct, std::forward_as_tuple(iAmount),
                        std::forward_as_tuple(iAmount, decade, digit));
      }
    }
  }

 public:
  static const std::unordered_map<uint64_t, AmountDecompositionUnit>& units() {
    static const AmountDecompositions __Singleton{};
    return __Singleton.m_units;
  }
};
}  // namespace

template <>
bool CryptoNote::toBinaryArray(const BinaryArray& object, BinaryArray& binaryArray) {
  try {
    Common::VectorOutputStream stream(binaryArray);
    BinaryOutputStreamSerializer serializer(stream);
    std::string oldBlob = Common::asString(object);
    XI_RETURN_EC_IF_NOT(serializer(oldBlob, ""), false);
  } catch (std::exception&) {
    return false;
  }

  return true;
}

uint64_t CryptoNote::getInputAmount(const Transaction& transaction) {
  uint64_t amount = 0;
  for (const auto& input : transaction.inputs) {
    if (auto keyInput = std::get_if<KeyInput>(&input)) {
      amount += keyInput->amount;
    }
  }

  return amount;
}

std::vector<uint64_t> CryptoNote::getInputsAmounts(const Transaction& transaction) {
  std::vector<uint64_t> inputsAmounts;
  inputsAmounts.reserve(transaction.inputs.size());

  for (auto& input : transaction.inputs) {
    if (auto keyInput = std::get_if<KeyInput>(&input)) {
      inputsAmounts.push_back(keyInput->amount);
    }
  }

  return inputsAmounts;
}

uint64_t CryptoNote::getOutputAmount(const TransactionPrefix& transaction) {
  uint64_t amount = 0;
  for (auto& output : transaction.outputs) {
    if (const auto amountOutput = std::get_if<TransactionAmountOutput>(std::addressof(output))) {
      amount += amountOutput->amount;
    }
  }

  return amount;
}

void CryptoNote::decomposeAmount(uint64_t amount, std::vector<uint64_t>& decomposedAmounts) {
  decompose_amount_into_digits(amount, [&](uint64_t amount) { decomposedAmounts.push_back(amount); });
}

size_t CryptoNote::countCanonicalDecomposition(const std::vector<uint64_t>& amounts) {
  size_t count = 0;
  std::array<size_t, std::numeric_limits<uint64_t>::digits10> decadeCount{};
  decadeCount.fill(0);
  for (const auto amount : amounts) {
    auto search = AmountDecompositions::units().find(amount);
    if (search == AmountDecompositions::units().end()) {
      return std::numeric_limits<uint32_t>::max();
    }
    const auto decadeIndex = search->second.DecadeIndex;
    decadeCount[decadeIndex] += 1;
    count = std::max(count, decadeCount[decadeIndex]);
  }
  return count;
}

bool CryptoNote::isCanonicalAmount(uint64_t amount) {
  return AmountDecompositions::units().find(amount) != AmountDecompositions::units().end();
}

size_t CryptoNote::countCanonicalDecomposition(const Transaction& tx) {
  std::vector<uint64_t> outs;
  outs.reserve(tx.outputs.size());
  for (const auto& output : tx.outputs) {
    if (const auto amountOutput = std::get_if<TransactionAmountOutput>(std::addressof(output))) {
      outs.push_back(amountOutput->amount);
    }
  }
  return countCanonicalDecomposition(outs);
}

uint64_t CryptoNote::cutDigitsFromAmount(uint64_t amount, const size_t count) {
  if (count == 0) {
    return amount;
  } else if (count >= std::numeric_limits<uint64_t>::digits10) {
    return 0;
  } else {
    uint64_t decimal = Xi::pow64(10, count);
    return amount - (amount % decimal);
  }
}

uint8_t CryptoNote::getCanoncialAmountDecimalPlace(uint64_t amount) {
  if (const auto search = AmountDecompositions::units().find(amount); search != AmountDecompositions::units().end()) {
    return search->second.DecadeIndex;
  } else {
    XI_RETURN_EC(std::numeric_limits<uint8_t>::max());
  }
}
