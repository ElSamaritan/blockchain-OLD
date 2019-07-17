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

#include <string>
#include <memory>
#include <utility>
#include <tuple>
#include <system_error>

#include <Xi/Error.h>

namespace CryptoNote {

namespace error {

enum class TransactionPoolError {
  SUCCESS = 0,
  BLOCKCHAIN_UNINITIALIZED = 1,
  NOT_FOUND = 2,
  MAIN_CHAIN_MISSING = 3,
  INPUT_UNLOCKS_TOO_FAR_IN_FUTURE = 4,
  ALREADY_MINED = 5,  ///< The transaction being pushed is already present in the main chain. This may occur if the main
                      ///< chain switches and both chains mined the transaction already.

  __NUM = 6
};

class TransactionPoolErrorCategory : public std::error_category {
 public:
  static TransactionPoolErrorCategory INSTANCE;

  virtual const char* name() const throw() {
    return "TransactionValidationErrorCategory";
  }

  virtual std::error_condition default_error_condition(int ev) const throw() {
    return std::error_condition(ev, *this);
  }

  virtual std::string message(int ev) const {
    TransactionPoolError code = static_cast<TransactionPoolError>(ev);

    switch (code) {
      case TransactionPoolError::SUCCESS:
        return "TransactionPool operation succeeded.";
      case TransactionPoolError::BLOCKCHAIN_UNINITIALIZED:
        return "The blockchain is currently not available for processing queries.";
      case TransactionPoolError::MAIN_CHAIN_MISSING:
        return "The main chain is missing, the blockchain is not initialized properly.";
      case TransactionPoolError::INPUT_UNLOCKS_TOO_FAR_IN_FUTURE:
        return "The transactions inputs used will unlock too far in future, please retry the transaction later.";
      case TransactionPoolError::NOT_FOUND:
        return "The queried transaction could not be found.";
      case TransactionPoolError::ALREADY_MINED:
        return "The transaction that is pushed is already present in the main chain.";
      default:
        return "Unknown error";
    }
  }

 private:
  TransactionPoolErrorCategory() = default;
};

inline std::error_code make_error_code(CryptoNote::error::TransactionPoolError e) {
  return std::error_code(static_cast<int>(e), CryptoNote::error::TransactionPoolErrorCategory::INSTANCE);
}

}  // namespace error
}  // namespace CryptoNote

namespace Xi {
inline Error make_error(CryptoNote::error::TransactionPoolError e) {
  return Error{make_error_code(e)};
}
}  // namespace Xi

namespace std {

template <>
struct is_error_code_enum<CryptoNote::error::TransactionPoolError> : public true_type {};

}  // namespace std
