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

#include "Xi/Error.h"

#include <stdexcept>

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
    return "Error wrapper contains unknown error type.";
  }
}

bool Xi::Error::isException() const { return m_error.type() == typeid(std::exception_ptr); }
std::exception_ptr Xi::Error::exception() const { return boost::get<std::exception_ptr>(m_error); }

bool Xi::Error::isErrorCode() const { return m_error.type() == typeid(std::error_code); }
std::error_code Xi::Error::errorCode() const { return boost::get<std::error_code>(m_error); }

void Xi::Error::throwException() const {
  if (isException()) {
    std::rethrow_exception(exception());
  } else {
    throw std::runtime_error{message()};
  }
}
