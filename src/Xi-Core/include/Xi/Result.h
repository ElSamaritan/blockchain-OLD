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

#pragma once

#include <memory>
#include <utility>
#include <type_traits>

#include <Xi/ExternalIncludePush.h>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Error.h>

namespace Xi {
/*!
 * \brief The Result class wraps an expected return value to be either the value or an error.
 *
 * This class enables methods to fail and propagate errors if one of their dependencies fail.
 *
 * \attention Whenever you return a result you may never throw an exception \see XI_ERROR_TRY and XI_ERROR CATCH
 *
 * \code{.cpp}
 * Xi::Result<Transaction> Proxy::queryTransaction(const Crypto::Hash& id) {
 *  XI_ERROR_TRY();
 *  // Assume we have a load balance that may return a result encoding an error or a valid remote serving the
 *  // transaction.
 *  auto remote = m_loadBalancer.queryTransactionRemote(id).takeOrThrow();
 *  return remote->queryTransaction(id);
 *  XI_ERROR_CATCH();
 * }
 * \endcode
 */
template <typename _ValueT>
class [[nodiscard]] Result {
 public:
  using value_t = typename std::decay<_ValueT>::type;

 private:
  boost::variant<Error, value_t> m_result;

  const value_t& value(std::true_type) const { return boost::get<value_t>(m_result); }
  value_t value(std::false_type) const { return boost::get<value_t>(m_result); }

 public:
  explicit Result() : m_result{Error{}} {}
  /* implicit */ Result(const Error& err) : m_result{err} {}
  /* implicit */ Result(value_t && value) : m_result{std::forward<value_t>(value)} {}

  XI_DEFAULT_COPY(Result);
  XI_DEFAULT_MOVE(Result);

  ~Result() = default;

  bool isValue() const { return m_result.type() == typeid(value_t); }
  bool isError() const { return m_result.type() != typeid(value_t); }

  const Error& error() const { return boost::get<Error>(m_result); }

  auto value() const { return value(typename std::is_compound<value_t>::type{}); }

  value_t take() {
    static_assert(std::is_move_constructible<value_t>::value, "You can only take move constructible types.");
    return std::move(boost::get<value_t>(m_result));
  }

  void throwOnError() {
    if (isError()) {
      error().throwException();
    }
  }

  value_t valueOrThrow() {
    throwOnError();
    return value();
  }

  value_t takeOrThrow() {
    throwOnError();
    return take();
  }
};

struct result_success {};

template <>
class [[nodiscard]] Result<void> {
 private:
  boost::optional<Error> m_error;

 public:
  explicit Result() : m_error{Error{}} {}
  /* implicit */ Result(result_success) : m_error{boost::none} {}
  /* implicit */ Result(const Error& err) : m_error{err} {}
  XI_DEFAULT_COPY(Result);
  XI_DEFAULT_MOVE(Result);
  ~Result() = default;

  bool isError() const { return m_error.is_initialized(); }

  const Error& error() const { return m_error.get(); }

  void throwOnError() {
    if (isError()) {
      error().throwException();
    }
  }
};

template <typename _ValueT, typename... _ArgsT>
inline Result<_ValueT> make_result(_ArgsT&&... args) {
  return Result<_ValueT>{typename Result<_ValueT>::value_t{std::forward<_ArgsT>(args)...}};
}

template <>
inline Result<void> make_result<void>() {
  return Result<void>{result_success{}};
}

}  // namespace Xi
