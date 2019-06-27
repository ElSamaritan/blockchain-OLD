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

#include <exception>
#include <system_error>
#include <variant>

#include <Xi/Global.hh>

namespace Xi {
/*!
 * \brief The Error class wraps an arbitrary error.
 *
 * \note An instance of an error is always an error. For conditionally return an error or nothing or a return value
 * \see Xi::Result .
 *
 * Sometimes caller are not able to handle any kind of error and may dont want to deal with them in any way except
 * returning immedietly. This often results into boolean returns or exceptions. The Error class provides an alternative
 * such that errors can be propagated backwards without loosing information about the error that actually occured.
 */
class [[nodiscard]] Error {
 public:
  struct not_initialized_tag {};

 public:
  /*!
   * \brief Error creates an not initialiezd error.
   *
   * Even if an error is not initialized properly it is considered as an error. This constructor is necessary to use
   * errors in many STL parts as they assume/require types to be default constructible.
   */
  explicit Error();

  /*!
   * \brief Error creates an error from an catched exception.
   * \param e The exception_ptr catched
   *
   * \code
   * try {
   *  ...
   *  return Xi::success();
   * } catch(...) {
   *  return Xi::make_error(std::current_exception());
   * }
   * \endcode
   */
  explicit Error(std::exception_ptr e);
  explicit Error(std::error_code ec);
  XI_DEFAULT_MOVE(Error);
  XI_DEFAULT_COPY(Error);
  ~Error() = default;

  /*!
   * \brief message yields a short description of the error.
   *
   * \todo may replace this with a contextual error message provider. That way we wont have to log errors until
   * we really want to and have necessary data stored to identify its source. Ie. a transaction validation fails
   * due to too low mixins. Then the provider could store max/min and actual mixin to provide a meaningfull error
   * message.
   */
  std::string message() const;

  /*!
   * \brief isException true if the underyling error is encoded as an exception.
   */
  bool isException() const;

  /*!
   * \brief exception the underyling exception error encoding.
   */
  std::exception_ptr exception() const;

  /*!
   * \brief isErrorCode true if the underlying error is encoded as an error code.
   */
  bool isErrorCode() const;

  /*!
   * \brief errorCode the underyling error code encoding.
   */
  std::error_code errorCode() const;

  /*!
   * \brief isNotInitialized the underlying erro was caused my a missing initialization.
   */
  bool isNotInitialized() const;

  /*!
   * \brief throwException Throws a runtime exception containing the error message.
   */
  [[noreturn]] void throwException() const;

 private:
  std::variant<not_initialized_tag, std::exception_ptr, std::error_code> m_error;
};

inline Error makeError(std::exception_ptr e) { return Error{e}; }
inline Error makeError(std::error_code ec) { return Error{ec}; }

}  // namespace Xi
