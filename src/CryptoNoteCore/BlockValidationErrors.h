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
#include <system_error>

namespace CryptoNote {
namespace error {

enum class BlockValidationError {
  VALIDATION_SUCCESS = 0,
  WRONG_VERSION = 1,
  WRONG_UPGRADE_VOTE = 2,
  PARENT_BLOCK_SIZE_TOO_BIG = 3,
  PARENT_BLOCK_WRONG_VERSION = 4,
  TIMESTAMP_TOO_FAR_IN_FUTURE = 5,
  TIMESTAMP_TOO_FAR_IN_PAST = 6,
  CUMULATIVE_BLOCK_SIZE_TOO_BIG = 7,
  DIFFICULTY_OVERHEAD = 8,
  BLOCK_REWARD_MISMATCH = 9,

  /// The hash of the static reward transaction must propagate the network. If a hash was emplaced but none
  /// expected or the emplaces hashed do not match the expected one the block is rejected.
  STATIC_REWARD_MISMATCH = 16,

  CHECKPOINT_BLOCK_HASH_MISMATCH = 10,
  PROOF_OF_WORK_TOO_WEAK = 11,
  TRANSACTION_ABSENT_IN_POOL = 12,
  TRANSACTION_DUPLICATES = 13,
  TRANSACTION_INCONSISTENCY = 14,
  UNEXPECTED_STATIC_REWARD = 15,

  __NUM = 17
};

// custom category:
class BlockValidationErrorCategory : public std::error_category {
 public:
  static BlockValidationErrorCategory INSTANCE;

  virtual const char* name() const noexcept { return "BlockValidationErrorCategory"; }

  virtual std::error_condition default_error_condition(int ev) const noexcept {
    return std::error_condition(ev, *this);
  }

  virtual std::string message(int ev) const {
    BlockValidationError code = static_cast<BlockValidationError>(ev);

    switch (code) {
      case BlockValidationError::VALIDATION_SUCCESS:
        return "Block validated successfully";
      case BlockValidationError::WRONG_VERSION:
        return "Wrong block version";
      case BlockValidationError::WRONG_UPGRADE_VOTE:
        return "Wrong block upgrade vote";
      case BlockValidationError::PARENT_BLOCK_SIZE_TOO_BIG:
        return "Parent block size is too big";
      case BlockValidationError::PARENT_BLOCK_WRONG_VERSION:
        return "Parent block has wrong version";
      case BlockValidationError::TIMESTAMP_TOO_FAR_IN_FUTURE:
        return "Timestamp is too far in future";
      case BlockValidationError::TIMESTAMP_TOO_FAR_IN_PAST:
        return "Timestamp is too far in past";
      case BlockValidationError::CUMULATIVE_BLOCK_SIZE_TOO_BIG:
        return "Cumulative block size is too big";
      case BlockValidationError::DIFFICULTY_OVERHEAD:
        return "Block difficulty overhead occurred";
      case BlockValidationError::BLOCK_REWARD_MISMATCH:
        return "Block reward doesn't match expected reward";
      case BlockValidationError::STATIC_REWARD_MISMATCH:
        return "Block static reward transaction hash does not match expected hash";
      case BlockValidationError::CHECKPOINT_BLOCK_HASH_MISMATCH:
        return "Checkpoint block hash mismatch";
      case BlockValidationError::PROOF_OF_WORK_TOO_WEAK:
        return "Proof of work is too weak";
      case BlockValidationError::TRANSACTION_ABSENT_IN_POOL:
        return "Block's transaction is absent in transaction pool";
      case BlockValidationError::TRANSACTION_DUPLICATES:
        return "Block contains duplicated transactions";
      case BlockValidationError::TRANSACTION_INCONSISTENCY:
        return "Block template and raw block have inconsistent transactions";
      case BlockValidationError::UNEXPECTED_STATIC_REWARD:
        return "Block template contains a static reward but static rewards are disabled for the current block version";
      default:
        return "Unknown error";
    }
  }

 private:
  BlockValidationErrorCategory() {}
};

inline std::error_code make_error_code(CryptoNote::error::BlockValidationError e) {
  return std::error_code(static_cast<int>(e), CryptoNote::error::BlockValidationErrorCategory::INSTANCE);
}

}  // namespace error
}  // namespace CryptoNote

namespace std {

template <>
struct is_error_code_enum<CryptoNote::error::BlockValidationError> : public true_type {};

}  // namespace std
