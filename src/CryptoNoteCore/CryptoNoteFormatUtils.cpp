﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
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

#include "CryptoNoteFormatUtils.h"

#include <set>
#include <Logging/LoggerRef.h>
#include <Common/Varint.h>

#include "Serialization/BinaryOutputStreamSerializer.h"
#include "Serialization/BinaryInputStreamSerializer.h"

#include "Account.h"
#include "CryptoNoteBasicImpl.h"
#include "CryptoNoteSerialization.h"
#include "Transactions/TransactionExtra.h"
#include "CryptoNoteTools.h"

#include <Xi/Config.h>

using namespace Logging;
using namespace Crypto;
using namespace Common;

namespace CryptoNote {

bool generate_key_image_helper(const AccountKeys& ack, const PublicKey& tx_public_key, size_t real_output_index,
                               KeyPair& in_ephemeral, KeyImage& ki) {
  KeyDerivation recv_derivation;
  bool r = generate_key_derivation(tx_public_key, ack.viewSecretKey, recv_derivation);

  assert(r && "key image helper: failed to generate_key_derivation");

  if (!r) {
    return false;
  }

  r = derive_public_key(recv_derivation, real_output_index, ack.address.spendPublicKey, in_ephemeral.publicKey);

  assert(r && "key image helper: failed to derive_public_key");

  if (!r) {
    return false;
  }

  derive_secret_key(recv_derivation, real_output_index, ack.spendSecretKey, in_ephemeral.secretKey);
  generate_key_image(in_ephemeral.publicKey, in_ephemeral.secretKey, ki);
  return true;
}

uint64_t power_integral(uint64_t a, uint64_t b) {
  if (b == 0)
    return 1;
  uint64_t total = a;
  for (uint64_t i = 1; i != b; i++)
    total *= a;
  return total;
}

bool get_tx_fee(const Transaction& tx, uint64_t& fee) {
  uint64_t amount_in = 0;
  uint64_t amount_out = 0;

  for (const auto& in : tx.inputs) {
    if (auto keyIn = std::get_if<KeyInput>(&in)) {
      amount_in += keyIn->amount;
    }
  }

  for (const auto& o : tx.outputs) {
    if (const auto amountOutput = std::get_if<TransactionAmountOutput>(std::addressof(o))) {
      amount_out += amountOutput->amount;
    }
  }

  if (!(amount_in >= amount_out)) {
    return false;
  }

  fee = amount_in - amount_out;
  return true;
}

uint64_t get_tx_fee(const Transaction& tx) {
  uint64_t r = 0;
  if (!get_tx_fee(tx, r))
    return 0;
  return r;
}

bool constructTransaction(const AccountKeys& sender_account_keys, const std::vector<TransactionSourceEntry>& sources,
                          const std::vector<TransactionDestinationEntry>& destinations, TransactionExtra extra,
                          Transaction& tx, uint64_t unlock_time, Logging::ILogger& log, const Currency& currency,
                          const BlockVersion blockVersion) {
  LoggerRef logger(log, "construct_tx");
  try {
    tx.inputs.clear();
    tx.outputs.clear();
    tx.signatures = TransactionSignatureCollection{};
    auto& signatures = std::get<TransactionSignatureCollection>(*tx.signatures);

    tx.version = 1;
    tx.type = TransactionType::Transfer;

    tx.unlockTime = unlock_time;
    if (tx.unlockTime > 0) {
      tx.features |= TransactionFeature::UniformUnlock;
    }

    tx.extra = extra;
    KeyPair txkey = generateKeyPair();
    addTransactionPublicKeyToExtra(tx.extra, txkey.publicKey);

    struct input_generation_context_data {
      KeyPair in_ephemeral;
    };

    uint64_t minGlobalIndex = std::numeric_limits<uint64_t>::max();
    uint64_t minRingSize = std::numeric_limits<uint64_t>::max();
    uint64_t maxRingSize = std::numeric_limits<uint64_t>::min();

    std::vector<input_generation_context_data> in_contexts;
    uint64_t summary_inputs_money = 0;
    // fill inputs
    for (const TransactionSourceEntry& src_entr : sources) {
      if (src_entr.realOutput >= src_entr.outputs.size()) {
        logger(Error) << "real_output index (" << src_entr.realOutput
                      << ")bigger than output_keys.size()=" << src_entr.outputs.size();
        return false;
      }
      summary_inputs_money += src_entr.amount;

      // KeyDerivation recv_derivation;
      in_contexts.push_back(input_generation_context_data());
      KeyPair& in_ephemeral = in_contexts.back().in_ephemeral;
      KeyImage img;
      if (!generate_key_image_helper(sender_account_keys, src_entr.realTransactionPublicKey,
                                     src_entr.realOutputIndexInTransaction, in_ephemeral, img))
        return false;

      // check that derived key is equal with real output key
      if (!(in_ephemeral.publicKey == src_entr.outputs[src_entr.realOutput].second)) {
        logger(Error) << "derived public key mismatch with output public key! " << ENDL
                      << "derived_key:" << Common::podToHex(in_ephemeral.publicKey) << ENDL
                      << "real output_public_key:" << Common::podToHex(src_entr.outputs[src_entr.realOutput].second);
        return false;
      }

      // put key image into tx input
      KeyInput input_to_key;
      input_to_key.amount = CanonicalAmount{src_entr.amount};
      input_to_key.keyImage = img;

      // fill outputs array and use relative offsets
      for (const TransactionSourceEntry::OutputEntry& out_entry : src_entr.outputs) {
        input_to_key.outputIndices.push_back(out_entry.first);
      }

      if (const auto sc = deltaEncodeGlobalIndices(input_to_key.outputIndices, input_to_key.outputIndices); !sc) {
        logger(Error) << "global output indices delta encoding failed" << ENDL;
        XI_RETURN_EC(false);
      }

      if (input_to_key.outputIndices.front() < minGlobalIndex) {
        minGlobalIndex = input_to_key.outputIndices.front();
      }
      const uint64_t ringSize = input_to_key.outputIndices.size();
      minRingSize = std::min(minRingSize, ringSize);
      maxRingSize = std::max(maxRingSize, ringSize);

      tx.inputs.push_back(input_to_key);
    }

    std::sort(tx.inputs.begin(), tx.inputs.end());

    // "Shuffle" outs
    std::vector<TransactionDestinationEntry> shuffled_dsts(destinations);
    std::sort(shuffled_dsts.begin(), shuffled_dsts.end(),
              [](const TransactionDestinationEntry& de1, const TransactionDestinationEntry& de2) {
                return de1.amount < de2.amount;
              });

    uint64_t summary_outs_money = 0;
    // fill outputs
    size_t output_index = 0;
    for (const TransactionDestinationEntry& dst_entr : shuffled_dsts) {
      if (!(dst_entr.amount > 0)) {
        logger(Error) << "Destination with wrong amount: " << dst_entr.amount;
        return false;
      }
      KeyDerivation derivation;
      PublicKey out_eph_public_key;
      bool r = generate_key_derivation(dst_entr.addr.viewPublicKey, txkey.secretKey, derivation);

      if (!(r)) {
        logger(Error) << "at creation outs: failed to generate_key_derivation(" << dst_entr.addr.viewPublicKey << ", "
                      << txkey.secretKey << ")";
        return false;
      }

      r = derive_public_key(derivation, output_index, dst_entr.addr.spendPublicKey, out_eph_public_key);
      if (!(r)) {
        logger(Error) << "at creation outs: failed to derive_public_key(" << derivation << ", " << output_index << ", "
                      << dst_entr.addr.spendPublicKey << ")";
        return false;
      }

      TransactionAmountOutput out;
      out.amount = CanonicalAmount{dst_entr.amount};
      KeyOutput tk;
      tk.key = out_eph_public_key;
      out.target = tk;
      tx.outputs.push_back(out);
      output_index++;
      summary_outs_money += dst_entr.amount;
    }

    // check money
    if (summary_outs_money > summary_inputs_money) {
      logger(Error) << "Transaction inputs money (" << summary_inputs_money << ") less than outputs money ("
                    << summary_outs_money << ")";
      return false;
    }

    bool isFusion = summary_outs_money == summary_inputs_money;
    TransactionFeature usableFeatures = TransactionFeature::None;
    if (isFusion) {
      usableFeatures = currency.transaction(blockVersion).fusion().features();
    } else {
      usableFeatures = currency.transaction(blockVersion).transfer().features();
    }

    if (minRingSize == maxRingSize && hasFlag(usableFeatures, TransactionFeature::StaticRingSize)) {
      tx.features |= TransactionFeature::StaticRingSize;
      tx.staticRingSize = static_cast<uint16_t>(minRingSize);
    }

    if (minGlobalIndex > 0 && hasFlag(usableFeatures, TransactionFeature::GlobalIndexOffset)) {
      tx.features |= TransactionFeature::GlobalIndexOffset;
      tx.globalIndexOffset = static_cast<GlobalIndex>(minGlobalIndex);
      for (auto& input : tx.inputs) {
        auto& amountInput = std::get<KeyInput>(input);
        amountInput.outputIndices.front() -= static_cast<GlobalDeltaIndex>(minGlobalIndex);
      }
    }

    // generate ring signatures
    Hash tx_prefix_hash = tx.prefixHash();

    size_t i = 0;
    for (const TransactionSourceEntry& src_entr : sources) {
      std::vector<const PublicKey*> keys_ptrs;
      for (const TransactionSourceEntry::OutputEntry& o : src_entr.outputs) {
        keys_ptrs.push_back(&o.second);
      }

      signatures.push_back(TransactionRingSignature{});
      auto& sigs = std::get<TransactionRingSignature>(signatures.back());
      sigs.resize(src_entr.outputs.size());
      generate_ring_signature(tx_prefix_hash, std::get<KeyInput>(tx.inputs[i]).keyImage, keys_ptrs,
                              in_contexts[i].in_ephemeral.secretKey, src_entr.realOutput, sigs.data());
      i++;
    }

    return true;
  } catch (std::exception& e) {
    logger(Logging::Fatal) << "unable to create transaction (EXCEPTIONAL): " << e.what();
    return false;
  }
}

bool getInputsMoneyAmount(const Transaction& tx, uint64_t& money) {
  money = 0;

  for (const auto& in : tx.inputs) {
    uint64_t amount = 0;

    if (auto keyIn = std::get_if<KeyInput>(&in)) {
      amount = keyIn->amount;
    }

    money += amount;
  }
  return true;
}

bool checkInputTypesSupported(const TransactionPrefix& tx) {
  for (const auto& in : tx.inputs) {
    if (!std::holds_alternative<KeyInput>(in)) {
      return false;
    }
  }
  return true;
}

bool checkOutsValid(const TransactionPrefix& tx, std::string* error) {
  for (const TransactionOutput& out : tx.outputs) {
    if (const auto amountOutput = std::get_if<TransactionAmountOutput>(std::addressof(out))) {
      if (auto keyOut = std::get_if<KeyOutput>(std::addressof(amountOutput->target))) {
        if (!isCanonicalAmount(amountOutput->amount)) {
          if (error) {
            *error = "None canonical amount";
          }
          return false;
        }

        if (!keyOut->key.isValid()) {
          if (error) {
            *error = "Output with invalid key";
          }
          return false;
        }
      } else {
        if (error) {
          *error = "Output with invalid type";
        }
        return false;
      }
    } else {
      return false;
    }
  }

  return true;
}

bool checkMoneyOverflow(const TransactionPrefix& tx) {
  return checkInputsOverflow(tx) && checkOutsOverflow(tx);
}

bool checkInputsOverflow(const TransactionPrefix& tx) {
  uint64_t money = 0;

  for (const auto& in : tx.inputs) {
    uint64_t amount = 0;

    if (auto keyInput = std::get_if<KeyInput>(&in)) {
      amount = keyInput->amount;
    }

    if (money > amount + money)
      return false;

    money += amount;
  }
  return true;
}

bool checkOutsOverflow(const TransactionPrefix& tx) {
  uint64_t money = 0;
  for (const auto& o : tx.outputs) {
    if (const auto amountOutput = std::get_if<TransactionAmountOutput>(std::addressof(o))) {
      if (money > amountOutput->amount + money)
        return false;
      money += amountOutput->amount;
    }
  }
  return true;
}

uint64_t get_outs_money_amount(const Transaction& tx) {
  uint64_t outputs_amount = 0;
  for (const auto& o : tx.outputs) {
    if (const auto amountOutput = std::get_if<TransactionAmountOutput>(std::addressof(o))) {
      outputs_amount += amountOutput->amount;
    }
  }
  return outputs_amount;
}

std::string short_hash_str(const Hash& h) {
  std::string res = Common::podToHex(h);

  if (res.size() == 64) {
    auto erased_pos = res.erase(8, 48);
    res.insert(8, "....");
  }

  return res;
}

bool is_out_to_acc(const AccountKeys& acc, const KeyOutput& out_key, const KeyDerivation& derivation, size_t keyIndex) {
  PublicKey pk;
  derive_public_key(derivation, keyIndex, acc.address.spendPublicKey, pk);
  return pk == out_key.key;
}

bool is_out_to_acc(const AccountKeys& acc, const KeyOutput& out_key, const PublicKey& tx_pub_key, size_t keyIndex) {
  KeyDerivation derivation;
  generate_key_derivation(tx_pub_key, acc.viewSecretKey, derivation);
  return is_out_to_acc(acc, out_key, derivation, keyIndex);
}

bool lookup_acc_outs(const AccountKeys& acc, const Transaction& tx, std::vector<size_t>& outs,
                     uint64_t& money_transfered) {
  PublicKey transactionPublicKey = getTransactionPublicKeyFromExtra(tx.extra);
  if (transactionPublicKey == PublicKey::Null)
    return false;
  return lookup_acc_outs(acc, tx, transactionPublicKey, outs, money_transfered);
}

bool lookup_acc_outs(const AccountKeys& acc, const Transaction& tx, const PublicKey& tx_pub_key,
                     std::vector<size_t>& outs, uint64_t& money_transfered) {
  money_transfered = 0;
  size_t keyIndex = 0;
  size_t outputIndex = 0;

  KeyDerivation derivation;
  generate_key_derivation(tx_pub_key, acc.viewSecretKey, derivation);

  for (const TransactionOutput& o : tx.outputs) {
    if (const auto amountOutput = std::get_if<TransactionAmountOutput>(std::addressof(o))) {
      if (auto keyOutput = std::get_if<KeyOutput>(std::addressof(amountOutput->target))) {
        if (is_out_to_acc(acc, *keyOutput, derivation, keyIndex)) {
          outs.push_back(outputIndex);
          money_transfered += amountOutput->amount;
        }
        ++keyIndex;
      }
    }

    ++outputIndex;
  }
  return true;
}

}  // namespace CryptoNote
