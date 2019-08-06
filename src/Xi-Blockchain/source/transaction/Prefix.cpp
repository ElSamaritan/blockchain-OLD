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

#include "Xi/Blockchain/Transaction/Prefix.hpp"

#include <Serialization/SerializationOverloads.h>
#include <CryptoNoteCore/CryptoNoteTools.h>

#include <numeric>

namespace Xi {
namespace Blockchain {
namespace Transaction {

Amount Prefix::generatedAmount() const {
  return std::accumulate(outputs.begin(), outputs.end(), 0ULL, [](const auto acc, const auto &output) {
    return acc + Xi::Blockchain::Transaction::generatedAmount(output);
  });
}

Amount Prefix::consumedAmount() const {
  return std::accumulate(inputs.begin(), inputs.end(), 0ULL, [](const auto acc, const auto &input) {
    return acc + Xi::Blockchain::Transaction::consumedAmount(input);
  });
}

bool Prefix::serialize(CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer(version, "version"), false);
  XI_RETURN_SC_IF(version == 0, true);
  XI_RETURN_EC_IF_NOT(version == 1, false);
  XI_RETURN_EC_IF_NOT(serializer(type, "type"), false);
  XI_RETURN_EC_IF_NOT(serializer(features, "features"), false);
  XI_RETURN_EC_IF_NOT(serializeUnlock(serializer), false);
  XI_RETURN_EC_IF_NOT(
      serializer.optional(hasFlag(features, Feature::GlobalIndexOffset), globalIndexOffset, "global_index_offset"),
      false);
  XI_RETURN_EC_IF(globalIndexOffset.value_or(1) == 0, false);
  XI_RETURN_EC_IF_NOT(
      serializer.optional(hasFlag(features, Feature::StaticRingSize), staticRingSize, "static_ring_size"), false);
  XI_RETURN_EC_IF(staticRingSize.value_or(1) == 0, false);
  XI_RETURN_EC_IF_NOT(serializer(extra, "extra"), false);

  if (serializer.isHumanReadable()) {
    XI_RETURN_EC_IF_NOT(serializer(inputs, "inputs"), false);
    XI_RETURN_EC_IF_NOT(serializer(outputs, "outputs"), false);
    XI_RETURN_SC(true);
  } else if (serializer.isMachinery()) {
    switch (type) {
      case Type::Reward:
        return serializeReward(serializer);

      case Type::Transfer:
        return serializeTransfer(serializer);

      case Type::None:
        XI_RETURN_EC(false);
    }

    XI_RETURN_EC(false);
  } else {
    XI_RETURN_EC(false);
  }
}

Crypto::FastHash Prefix::prefixHash() const {
  return Crypto::FastHash::computeObjectHash<Prefix>(*this).takeOrThrow();
}

uint64_t Prefix::prefixBinarySize() const {
  return CryptoNote::toBinaryArray<Prefix>(*this).size();
}

void Prefix::nullifyPrefix() {
  version = 0;
  type = Type::None;
  features = Feature::None;
  unlockTime = 0;
  inputs.clear();
  outputs.clear();
  extra.nullify();
}

bool Prefix::serializeUnlock(CryptoNote::ISerializer &serializer) {
  if (serializer.isInput()) {
    if (hasFlag(features, Feature::UniformUnlock)) {
      XI_RETURN_EC_IF_NOT(serializer(unlockTime, "unlock_time"), false);
      XI_RETURN_EC_IF(unlockTime == 0, false);
    } else {
      unlockTime = 0;
    }
  } else {
    if (hasFlag(features, Feature::UniformUnlock)) {
      XI_RETURN_EC_IF(unlockTime == 0, false);
      XI_RETURN_EC_IF_NOT(serializer(unlockTime, "unlock_time"), false);
    } else {
      XI_RETURN_EC_IF_NOT(unlockTime == 0, false);
    }
  }
  XI_RETURN_SC(true);
}

bool Prefix::serializeReward(CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer.beginStaticArray(1, "inputs"), false);
  if (serializer.isInput()) {
    RewardInput input{};
    XI_RETURN_EC_IF_NOT(serializer(input, ""), false);
    inputs.clear();
    inputs.emplace_back(Input{std::move(input)});
  } else if (serializer.isOutput()) {
    XI_RETURN_EC_IF_NOT(inputs.size() == 1, false);
    const auto reward = std::get_if<RewardInput>(std::addressof(inputs.front()));
    XI_RETURN_EC_IF(reward == nullptr, false);
    XI_RETURN_EC_IF_NOT(serializer(*reward, ""), false);
  } else {
    XI_RETURN_EC(false);
  }
  XI_RETURN_EC_IF_NOT(serializer.endArray(), false);
  return serializeOutputs(serializer);
}

bool Prefix::serializeTransfer(CryptoNote::ISerializer &serializer) {
  size_t inputCount = inputs.size();
  XI_RETURN_EC_IF_NOT(serializer.beginArray(inputCount, "inputs"), false);
  if (serializer.isInput()) {
    inputs = InputVector{inputCount, AmountInput{}};
  } else if (serializer.isOutput()) {
    for (const auto &input : inputs) {
      XI_RETURN_EC_IF_NOT(std::holds_alternative<AmountInput>(input), false);
    }
  } else {
    XI_RETURN_EC(false);
  }
  XI_RETURN_EC_IF(inputs.empty(), false);
  for (auto &input : inputs) {
    XI_RETURN_EC_IF_NOT(serializer.beginObject(""), false);
    XI_RETURN_EC_IF_NOT(std::get<AmountInput>(input).serialize(serializer, staticRingSize), false);
    XI_RETURN_EC_IF_NOT(serializer.endObject(), false);
  }
  XI_RETURN_EC_IF_NOT(serializer.endArray(), false);
  return serializeOutputs(serializer);
}

bool Prefix::serializeOutputs(CryptoNote::ISerializer &serializer) {
  size_t outputCount = outputs.size();
  XI_RETURN_EC_IF_NOT(serializer.beginArray(outputCount, "outputs"), false);
  if (serializer.isInput()) {
    outputs = OutputVector{outputCount, AmountOutput{CanonicalAmount{0}, KeyOutputTarget{}}};
  } else if (serializer.isOutput()) {
    for (const auto &output : outputs) {
      XI_RETURN_EC_IF_NOT(std::holds_alternative<AmountOutput>(output), false);
      const auto &amountOutput = std::get<AmountOutput>(output);
      XI_RETURN_EC_IF_NOT(std::holds_alternative<KeyOutputTarget>(amountOutput.target), false);
    }
  } else {
    XI_RETURN_EC(false);
  }
  XI_RETURN_EC_IF(outputs.empty(), false);
  for (auto &output : outputs) {
    XI_RETURN_EC_IF_NOT(serializer.beginObject(""), false);
    auto &amountOutput = std::get<AmountOutput>(output);
    XI_RETURN_EC_IF_NOT(serializer(amountOutput.amount, "amount"), false);
    auto &keyTarget = std::get<KeyOutputTarget>(amountOutput.target);
    XI_RETURN_EC_IF_NOT(serializer(keyTarget.key, "key"), false);
    XI_RETURN_EC_IF_NOT(serializer.endObject(), false);
  }
  XI_RETURN_EC_IF_NOT(serializer.endArray(), false);
  XI_RETURN_SC(true);
}

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi
