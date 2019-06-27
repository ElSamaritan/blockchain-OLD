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

#pragma once

#include <cinttypes>
#include <vector>

#include <Xi/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <Xi/ExternalIncludePop.h>

#include "CryptoNoteCore/CryptoNoteBasic.h"
#include "CryptoNoteCore/Transactions/ITransactionBuilder.h"

namespace CryptoNote {

bool checkInputsKeyimagesDiff(const CryptoNote::TransactionPrefix& tx);

// TransactionInput helper functions
size_t getRequiredSignaturesCount(const TransactionInput& in);

uint64_t getTransactionInputAmount(const TransactionInput& in);
uint64_t getTransactionInputAmount(const Transaction& transaction);

uint64_t getTransactionOutputAmount(const TransactionOutput& out);
uint64_t getTransactionOutputAmount(const Transaction& transaction);

boost::optional<Crypto::KeyImage> getTransactionInputKeyImage(const TransactionInput& input);
std::vector<Crypto::KeyImage> getTransactionKeyImages(const Transaction& transaction);

Crypto::PublicKey getTransactionOutputKey(const TransactionOutputTarget& target);
Crypto::PublicKey getTransactionOutputKey(const TransactionOutput& output);
std::vector<Crypto::PublicKey> getTransactionOutputKeys(const Transaction& transaction);

std::vector<uint32_t> getTransactionInputIndices(const KeyInput& input);

TransactionTypes::InputType getTransactionInputType(const TransactionInput& in);
const TransactionInput& getInputChecked(const CryptoNote::TransactionPrefix& transaction, size_t index);
const TransactionInput& getInputChecked(const CryptoNote::TransactionPrefix& transaction, size_t index,
                                        TransactionTypes::InputType type);

bool isOutToKey(const Crypto::PublicKey& spendPublicKey, const Crypto::PublicKey& outKey,
                const Crypto::KeyDerivation& derivation, size_t keyIndex);

// TransactionOutput helper functions
TransactionTypes::OutputType getTransactionOutputType(const TransactionOutputTarget& out);
const TransactionOutput& getOutputChecked(const CryptoNote::TransactionPrefix& transaction, size_t index);
const TransactionOutput& getOutputChecked(const CryptoNote::TransactionPrefix& transaction, size_t index,
                                          TransactionTypes::OutputType type);

bool findOutputsToAccount(const CryptoNote::TransactionPrefix& transaction, const AccountPublicAddress& addr,
                          const Crypto::SecretKey& viewSecretKey, std::vector<uint32_t>& out, uint64_t& amount);

}  // namespace CryptoNote
