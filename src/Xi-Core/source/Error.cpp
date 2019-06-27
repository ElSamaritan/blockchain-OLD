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

#include "Xi/Error.h"

#include <stdexcept>
#include <cassert>

Xi::Error::Error() : m_error{Error::not_initialized_tag{}} {}
Xi::Error::Error(std::exception_ptr e) : m_error{e} {}
Xi::Error::Error(std::error_code ec) : m_error{ec} {}

std::string Xi::Error::message() const {
  if (isException()) {
    try {
      std::rethrow_exception(exception());
    } catch (const std::exception& e) {
      return e.what();
    } catch (...) {
      return "Unknown type has been thrown.";
    }
  } else if (isErrorCode()) {
    return errorCode().message();
  } else {
    assert(isNotInitialized());
    return "The underlying result was not initialized.";
  }
}

bool Xi::Error::isException() const { return std::holds_alternative<std::exception_ptr>(m_error); }
std::exception_ptr Xi::Error::exception() const { return std::get<std::exception_ptr>(m_error); }

bool Xi::Error::isErrorCode() const { return std::holds_alternative<std::error_code>(m_error); }
std::error_code Xi::Error::errorCode() const { return std::get<std::error_code>(m_error); }

void Xi::Error::throwException() const {
  if (isException()) {
    std::rethrow_exception(exception());
  } else if (isErrorCode()) {
    throw std::runtime_error{message()};
  } else {
    assert(isNotInitialized());
    throw std::runtime_error{message()};
  }
}

bool Xi::Error::isNotInitialized() const { return std::holds_alternative<not_initialized_tag>(m_error); }
