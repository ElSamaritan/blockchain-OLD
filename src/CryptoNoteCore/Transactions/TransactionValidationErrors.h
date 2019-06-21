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

#pragma once

#include <string>
#include <memory>
#include <utility>
#include <tuple>
#include <system_error>

#include <Xi/Error.h>

namespace CryptoNote {

namespace error {

enum class TransactionValidationError {
  VALIDATION_SUCCESS = 0,  ///< The validation succeeded.

  INVALID_BINARY_REPRESNETATION = 1,  ///< A binary blob was provided which could not be parsed into a transaction

  EXISTS_IN_BLOCKCHAIN = 2,  ///< The transaction was already mined
  EXISTS_IN_POOL = 3,        ///< The transaction is already present in the pool.
  TOO_LARGE = 4,             ///< The binary size of the transaction is too large.
  TOO_LARGE_FOR_REWARD_ZONE =
      35,              ///< The pool denied the transaction being to large to get a sufficient reward for mining it.
  EMPTY_INPUTS = 5,    ///< The transaction has no inputs.
  EMPTY_OUTPUTS = 47,  ///< The transaction has no outputs.
  INPUT_UNKNOWN_TYPE = 6,
  INPUT_EMPTY_OUTPUT_USAGE = 7,        ///< One of the inputs has no outputs in the transaction and would disapper.
  INPUT_INVALID_DOMAIN_KEYIMAGES = 8,  ///< The identifier to protect double spending was computed using a wrong domain.
                                       ///< That way a user could create different key images for the same input and
                                       ///< double spending could not be detected.
  INPUT_IDENTICAL_KEYIMAGES = 9,       ///< The transaction contains a double spending
  INPUT_IDENTICAL_OUTPUT_INDEXES = 10,
  INPUT_KEYIMAGE_ALREADY_SPENT = 11,
  INPUT_INVALID_GLOBAL_INDEX = 12,  ///< The input used could not be found.
  INPUT_SPEND_LOCKED_OUT =
      13,  ///< An input used for the transaction is an output of a transaction that is still locked.
  EXTRA_MISSING_PUBLIC_KEY = 41,  ///< Every transaction must have at least one embedded public key.
  EXTRA_INVALID_PUBLIC_KEY = 42,  ///< A transaction extra encoded public key must be a valid ecc point.
  EXTRA_INVALID_NONCE = 46,  ///< A transaction extra nonce, may encode a single playment id or custom data indicated by
                             ///< the custom data start flag.
  INPUT_INVALID_SIGNATURES = 14,  ///< The signing signature of the input is invalid. Its likely that a user tried to
                                  ///< use coins he does not own.
  INPUT_WRONG_SIGNATURES_COUNT = 15,
  INPUTS_AMOUNT_OVERFLOW = 16,         ///< The sum of all amounts of the transaction input caused a memory overflow
  BASE_INPUT_WRONG_COUNT = 17,         ///< The miner reward transaction has an invalid amount of inputs
  BASE_INPUT_WRONG_BLOCK_INDEX = 18,   ///< The encoded block index of the miner reward input is wrong.
  BASE_INPUT_UNEXPECTED_TYPE = 19,     ///< The type of the miner reward input transaction has an unexpected type that
                                       ///< cannot be processes.
  BASE_INPUT_INVALID_NONCE = 37,       ///< Base transactions can only store public keys in their nonce.
  BASE_INVALID_SIGNATURES_COUNT = 38,  ///< Base transaction may not contain any signatures.
  OUTPUTS_NOT_CANONCIAL = 45,          ///< Base transaction may always have the canonical form.
  UNUSED_STATIC_REWARD_INVALID_ADDRESS = 39,  ///< The static reward address is invalid, not the expected one.
  UNUSED_STATIC_REWARD_INVALID_OUT = 40,  ///< The static reward contains an invalid out, either wrong encoded or not
                                          ///< designated to the built in static reward address.
  EXTRA_NONCE_TOO_LARGE =
      36,                 ///< The extra nonce of the transaction is larger than allowed (TX_EXTRA_NONCE_MAX_COUNT).
  EXTRA_TOO_LARGE = 43,   ///< Extra size exceeds threshold
  EXTRA_ILL_FORMED = 44,  ///< Extra could not be parsed
  INPUT_AMOUNT_INSUFFICIENT = 20,       ///< The sum of inputs to the transaction is lower than the sum of outputs.
  INPUT_INVALID_SIGNATURES_COUNT = 21,  ///< The number of signatures to sign each input/output pair is not equal to the
                                        ///< number of outputs.
  INPUT_INVALID_UNKNOWN =
      22,                   ///< An error occured during input validation thats has no mapping to this error collection.
  FEE_INSUFFICIENT = 23,    ///< A transaction was pushed to the transaction pool with unsifficient fees.
  OUTPUT_ZERO_AMOUNT = 24,  ///< One of the outputs actually has no amount attached to it
  OUTPUT_INVALID_KEY = 25,
  OUTPUT_INVALID_REQUIRED_SIGNATURES_COUNT = 26,
  OUTPUT_UNEXPECTED_TYPE = 27,
  OUTPUT_UNKNOWN_TYPE = 28,
  OUTPUTS_AMOUNT_OVERFLOW = 29,             ///< The sum of amounts of all outputs caused a memory overflow
  BASE_TRANSACTION_WRONG_UNLOCK_TIME = 30,  ///< Only used for base transactions, if the miner has not used the correct
                                            ///< unlocking window
  INVALID_VERSION = 31,  ///< The version is not supported. This protects against user trying to use new transaction
                         ///< versions that are currently not supported.
  INVALID_MIXIN = 32,
  INPUT_MIXIN_TOO_HIGH = 33,
  INPUT_MIXIN_TOO_LOW = 34,

  __NUM = 48  ///< The count of different enum values, if you add a new one use this as its value and increase this by
              ///< one. Do not reorder assignments as it would lead to inconsistent error codes in the documentation
              ///< and tickets aso.
};

// custom category:
class TransactionValidationErrorCategory : public std::error_category {
 public:
  static TransactionValidationErrorCategory INSTANCE;

  virtual const char* name() const noexcept { return "TransactionValidationErrorCategory"; }

  virtual std::error_condition default_error_condition(int ev) const noexcept {
    return std::error_condition(ev, *this);
  }

  virtual std::string message(int ev) const {
    TransactionValidationError code = static_cast<TransactionValidationError>(ev);

    switch (code) {
      case TransactionValidationError::VALIDATION_SUCCESS:
        return "Transaction successfully validated";
      case TransactionValidationError::INVALID_BINARY_REPRESNETATION:
        return "Failed to parse transaction blob";
      case TransactionValidationError::EMPTY_INPUTS:
        return "Transaction has no inputs";
      case TransactionValidationError::EMPTY_OUTPUTS:
        return "Transaction has no outputs";
      case TransactionValidationError::INPUT_UNKNOWN_TYPE:
        return "Transaction has input with unknown type";
      case TransactionValidationError::INPUT_EMPTY_OUTPUT_USAGE:
        return "Transaction's input uses empty output";
      case TransactionValidationError::INPUT_INVALID_DOMAIN_KEYIMAGES:
        return "Transaction uses key image not in the valid domain";
      case TransactionValidationError::INPUT_IDENTICAL_KEYIMAGES:
        return "Transaction has identical key images";
      case TransactionValidationError::INPUT_IDENTICAL_OUTPUT_INDEXES:
        return "Transaction has identical output indexes";
      case TransactionValidationError::INPUT_KEYIMAGE_ALREADY_SPENT:
        return "Transaction key image is already spent.";
      case TransactionValidationError::INPUT_INVALID_GLOBAL_INDEX:
        return "Transaction has input with invalid global index";
      case TransactionValidationError::INPUT_SPEND_LOCKED_OUT:
        return "Transaction uses locked input";
      case TransactionValidationError::INPUT_INVALID_SIGNATURES:
        return "Transaction has input with invalid signature";
      case TransactionValidationError::INPUT_WRONG_SIGNATURES_COUNT:
        return "Transaction has input with wrong signatures count";
      case TransactionValidationError::INPUTS_AMOUNT_OVERFLOW:
        return "Transaction's inputs sum overflow";
      case TransactionValidationError::BASE_INPUT_WRONG_COUNT:
        return "Wrong input count";
      case TransactionValidationError::BASE_INPUT_UNEXPECTED_TYPE:
        return "Wrong input type";
      case TransactionValidationError::BASE_INPUT_INVALID_NONCE:
        return "The base input transaction has an invalid encoded nonce";
      case TransactionValidationError::BASE_INVALID_SIGNATURES_COUNT:
        return "Base transaction contains signature, which is prohibited.";
      case TransactionValidationError::UNUSED_STATIC_REWARD_INVALID_ADDRESS:
        return "Static reward contains an unexpected public key.";
      case TransactionValidationError::UNUSED_STATIC_REWARD_INVALID_OUT:
        return "Static reward contains an invalid output.";
      case TransactionValidationError::INPUT_AMOUNT_INSUFFICIENT:
        return "Transaction contains more output than input";
      case TransactionValidationError::INPUT_INVALID_SIGNATURES_COUNT:
        return "Transaction has a wrong amount of signatures attached.";
      case TransactionValidationError::INPUT_INVALID_UNKNOWN:
        return "Transaction input validation yielded an error that is not known to the transaction validation routine.";
      case TransactionValidationError::FEE_INSUFFICIENT:
        return "Transaction fees are insufficient";
      case TransactionValidationError::BASE_INPUT_WRONG_BLOCK_INDEX:
        return "Base input has wrong block index";
      case TransactionValidationError::OUTPUT_ZERO_AMOUNT:
        return "Transaction has zero output amount";
      case TransactionValidationError::OUTPUT_INVALID_KEY:
        return "Transaction has output with invalid key";
      case TransactionValidationError::OUTPUT_INVALID_REQUIRED_SIGNATURES_COUNT:
        return "Transaction has output with invalid signatures count";
      case TransactionValidationError::OUTPUT_UNEXPECTED_TYPE:
        return "Transaction has unexpected output type";
      case TransactionValidationError::OUTPUT_UNKNOWN_TYPE:
        return "Transaction has unknown output type";
      case TransactionValidationError::OUTPUTS_AMOUNT_OVERFLOW:
        return "Transaction has outputs amount overflow";
      case TransactionValidationError::BASE_TRANSACTION_WRONG_UNLOCK_TIME:
        return "Transaction has wrong unlock time";
      case TransactionValidationError::INVALID_MIXIN:
        return "Mixin too large or too small";
      case TransactionValidationError::INVALID_VERSION:
        return "Transaction has an unknown version";
      case TransactionValidationError::TOO_LARGE:
        return "The binary size of the transaction is too large.";
      case TransactionValidationError::TOO_LARGE_FOR_REWARD_ZONE:
        return "The pool denied the transaction being to large to get a sufficient reward for mining it.";
      case TransactionValidationError::EXTRA_INVALID_PUBLIC_KEY:
        return "Transaction public key is invalid.";
      case TransactionValidationError::EXTRA_MISSING_PUBLIC_KEY:
        return "No public key found in extra.";
      case TransactionValidationError::EXISTS_IN_BLOCKCHAIN:
        return "Transaction is already mined.";
      case TransactionValidationError::EXISTS_IN_POOL:
        return "Transaction is already in pool.";
      case TransactionValidationError::INPUT_MIXIN_TOO_LOW:
        return "Mixin is too low.";
      case TransactionValidationError::INPUT_MIXIN_TOO_HIGH:
        return "Mixin is too high.";
      case TransactionValidationError::EXTRA_TOO_LARGE:
        return "Extra excceds maximum size.";
      case TransactionValidationError::EXTRA_NONCE_TOO_LARGE:
        return "Extra nonce exceeds maximum size.";
      case TransactionValidationError::EXTRA_ILL_FORMED:
        return "Extra is ill formed.";
      case TransactionValidationError::EXTRA_INVALID_NONCE:
        return "Extra nonce is ill formed.";
      case TransactionValidationError::OUTPUTS_NOT_CANONCIAL:
        return "Outputs are not in canoncial form.";
      default:
        return "Unknown error";
    }
  }

 private:
  TransactionValidationErrorCategory() {}
};

inline std::error_code make_error_code(CryptoNote::error::TransactionValidationError e) {
  return std::error_code(static_cast<int>(e), CryptoNote::error::TransactionValidationErrorCategory::INSTANCE);
}

}  // namespace error
}  // namespace CryptoNote

namespace Xi {
inline Error make_error(CryptoNote::error::TransactionValidationError e) {
  return Error{CryptoNote::error::make_error_code(e)};
}
}  // namespace Xi

namespace std {

template <>
struct is_error_code_enum<CryptoNote::error::TransactionValidationError> : public true_type {};

}  // namespace std
