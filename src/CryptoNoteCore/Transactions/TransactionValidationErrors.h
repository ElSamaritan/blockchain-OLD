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

namespace CryptoNote {

namespace error {

enum class TransactionValidationError {
  VALIDATION_SUCCESS = 0,  ///< The validation succeeded.
  TOO_LARGE,               ///< The binary size of the transaction is too large.
  EMPTY_INPUTS,            ///< The transaction has no inputs.
  INPUT_UNKNOWN_TYPE,
  INPUT_EMPTY_OUTPUT_USAGE,        ///< One of the inputs has no outputs in the transaction and would disapper.
  INPUT_INVALID_DOMAIN_KEYIMAGES,  ///< The identifier to protect double spending was computed using a wrong domain.
                                   ///< That way a user could create different key images for the same input and double
                                   ///< spending could not be detected.
  INPUT_IDENTICAL_KEYIMAGES,       ///< The transaction contains a double spending
  INPUT_IDENTICAL_OUTPUT_INDEXES,
  INPUT_KEYIMAGE_ALREADY_SPENT,
  INPUT_INVALID_GLOBAL_INDEX,  ///< The input used could not be found.
  INPUT_SPEND_LOCKED_OUT,    ///< An input used for the transaction is an output of a transaction that is still locked.
  INPUT_INVALID_SIGNATURES,  ///< The signing signature of the input is invalid. Its likely that a user tried to use
                             ///< coins he does not own.
  INPUT_WRONG_SIGNATURES_COUNT,
  INPUTS_AMOUNT_OVERFLOW,        ///< The sum of all amounts of the transaction input caused a memory overflow
  BASE_INPUT_WRONG_COUNT,        ///< The miner reward transaction has an invalid amount of inputs
  BASE_INPUT_WRONG_BLOCK_INDEX,  ///< The encoded block index of the miner reward input is wrong.
  BASE_INPUT_UNEXPECTED_TYPE,  ///< The type of the miner reward input transaction has an unexpected type that cannot be
                               ///< processes.
  INPUT_AMOUNT_INSUFFICIENT,   ///< The sum of inputs to the transaction is lower than the sum of outputs.
  INPUT_INVALID_SIGNATURES_COUNT,  ///< The number of signatures to sign each input/output pair is not equal to the
                                   ///< number of outputs.
  INPUT_INVALID_UNKNOWN,  ///< An error occured during input validation thats has no mapping to this error collection.
  FEE_INSUFFICIENT,       ///< A transaction was pushed to the transaction pool with unsifficient fees.
  OUTPUT_ZERO_AMOUNT,     ///< One of the outputs actually has no amount attached to it
  OUTPUT_INVALID_KEY,
  OUTPUT_INVALID_REQUIRED_SIGNATURES_COUNT,
  OUTPUT_UNEXPECTED_TYPE,
  OUTPUT_UNKNOWN_TYPE,
  OUTPUTS_AMOUNT_OVERFLOW,             ///< The sum of amounts of all outputs caused a memory overflow
  BASE_TRANSACTION_WRONG_UNLOCK_TIME,  ///< Only used for base transactions, if the miner has not used the correct
                                       ///< unlocking window
  INVALID_VERSION,  ///< The version is not supported. This protects against user trying to use new transaction versions
                    ///< that are currently not supported.
  INVALID_MIXIN,
  INPUT_MIXIN_TOO_HIGH,
  INPUT_MIXIN_TOO_LOW,
};

// custom category:
class TransactionValidationErrorCategory : public std::error_category {
 public:
  static TransactionValidationErrorCategory INSTANCE;

  virtual const char* name() const throw() { return "TransactionValidationErrorCategory"; }

  virtual std::error_condition default_error_condition(int ev) const throw() { return std::error_condition(ev, *this); }

  virtual std::string message(int ev) const {
    TransactionValidationError code = static_cast<TransactionValidationError>(ev);

    switch (code) {
      case TransactionValidationError::VALIDATION_SUCCESS:
        return "Transaction successfully validated";
      case TransactionValidationError::EMPTY_INPUTS:
        return "Transaction has no inputs";
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
        return "Transaction key image is already spent in another transaction";
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
      case TransactionValidationError::INPUT_AMOUNT_INSUFFICIENT:
        return "Transaction contains more output than input";
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

namespace std {

template <>
struct is_error_code_enum<CryptoNote::error::TransactionValidationError> : public true_type {};

}  // namespace std
