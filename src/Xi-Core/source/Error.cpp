/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

Xi::Error::Error(std::exception_ptr e) : m_error{e} {}
Xi::Error::Error(std::error_code ec) : m_error{ec} {}

std::string Xi::Error::message() const {
  if (m_error.type() == typeid(std::exception_ptr)) {
    auto e = boost::get<std::exception_ptr>(m_error);
    try {
      std::rethrow_exception(e);
    } catch (const std::exception& e) {
      return e.what();
    } catch (...) {
      return "Unknown type has been thrown.";
    }
  } else if (m_error.type() == typeid(std::error_code)) {
    auto ec = boost::get<std::error_code>(m_error);
    return ec.message();
  } else {
    return "Error wrapper contains unknown error type.";
  }
}
