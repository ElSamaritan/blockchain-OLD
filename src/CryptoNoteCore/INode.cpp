/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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

#include "CryptoNoteCore/INode.h"

#include <memory>
#include <type_traits>

// Please note the future callback wrappers are only valid if the callback receiver uses the std::function wrapper.
static_assert(std::is_same_v<CryptoNote::INode::Callback, std::function<void(std::error_code)>>, "");

std::future<Xi::Result<void>> CryptoNote::INode::init() {
  auto promise = std::make_shared<std::promise<Xi::Result<void>>>();
  auto future = promise->get_future();
  init([promise](std::error_code ec) mutable {
    if (ec) {
      promise->set_value(Xi::failure(ec));
    } else {
      promise->set_value(Xi::success());
    }
  });
  return future;
}

std::future<Xi::Result<CryptoNote::BlockHeaderInfo>> CryptoNote::INode::getLastBlockHeaderInfo() {
  auto promise = std::make_shared<std::promise<Xi::Result<BlockHeaderInfo>>>();
  auto reval = std::make_shared<BlockHeaderInfo>();
  auto future = promise->get_future();
  getLastBlockHeaderInfo(*reval, [promise, reval](std::error_code ec) mutable {
    if (ec) {
      promise->set_value(Xi::failure(ec));
    } else {
      promise->set_value(Xi::success(std::move(*reval)));
    }
  });
  return future;
}

std::future<Xi::Result<std::vector<CryptoNote::RawBlock>>> CryptoNote::INode::getRawBlocksByRange(BlockHeight height,
                                                                                                  uint32_t count) {
  auto promise = std::make_shared<std::promise<Xi::Result<std::vector<RawBlock>>>>();
  auto reval = std::make_shared<std::vector<RawBlock>>();
  auto future = promise->get_future();
  getRawBlocksByRange(height, count, *reval, [promise, reval](std::error_code ec) mutable {
    if (ec) {
      promise->set_value(Xi::failure(ec));
    } else {
      promise->set_value(Xi::success(std::move(*reval)));
    }
  });
  return future;
}
