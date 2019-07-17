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

#include <memory>
#include <utility>
#include <type_traits>
#include <variant>
#include <optional>

#include <Xi/Error.h>

namespace Xi {

struct result_success {};
struct result_failure {};

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
  using value_t = std::decay_t<_ValueT>;

 private:
  std::variant<Error, value_t> m_result;

 public:
  explicit Result() : m_result{std::in_place_index<0>, Error{}} {
  }
  explicit Result(result_failure) : m_result{std::in_place_index<0>, Error{}} {
  }
  /* implicit */ Result(const Error& err) : m_result{std::in_place_index<0>, err} {
  }
  Result(result_success, value_t value) : m_result{std::in_place_index<1>, std::forward<value_t>(value)} {
  }

  ~Result() = default;

  bool isError() const {
    return this->m_result.index() == 0;
  }
  bool isValue() const {
    return this->m_result.index() == 1;
  }

  const Error& error() const {
    return std::get<Error>(m_result);
  }

  const value_t& value() const {
    return std::get<1>(m_result);
  }
  const value_t& operator*() const {
    return std::get<1>(m_result);
  }
  value_t& value() {
    return std::get<1>(m_result);
  }
  value_t& operator*() {
    return std::get<1>(m_result);
  }

  value_t take() {
    static_assert(std::is_move_constructible<value_t>::value, "You can only take move constructible types.");
    return std::move(std::get<1>(m_result));
  }

  void throwOnError() {
    if (isError()) {
      error().throwException();
    }
  }

  value_t& valueOrThrow() {
    throwOnError();
    return this->value();
  }
  const value_t& valueOrThrow() const {
    throwOnError();
    return this->value();
  }

  value_t takeOrThrow() {
    throwOnError();
    return this->take();
  }

  const value_t* operator->() const {
    return std::get_if<value_t>(&this->m_result);
  }
  value_t* operator->() {
    return std::get_if<value_t>(&this->m_result);
  }
};

template <>
class [[nodiscard]] Result<void> {
 private:
  std::optional<Error> m_error;

 public:
  explicit Result() : m_error{Error{}} {
  }
  explicit Result(result_success) : m_error{std::nullopt} {
  }
  /* implicit */ Result(const Error& err) : m_error{err} {
  }
  XI_DEFAULT_COPY(Result);
  XI_DEFAULT_MOVE(Result);
  ~Result() = default;

  bool isError() const {
    return m_error.has_value();
  }

  const Error& error() const {
    return *m_error;
  }

  void throwOnError() {
    if (isError()) {
      error().throwException();
    }
  }
};

inline Error failure(std::exception_ptr e) {
  return makeError(e);
}
inline Error failure(std::error_code e) {
  return makeError(e);
}

template <typename _ValueT>
inline Result<std::decay_t<_ValueT>> success(_ValueT&& val) {
  return Result<std::decay_t<_ValueT>>{result_success{}, typename Result<_ValueT>::value_t{std::forward<_ValueT>(val)}};
}

template <typename _ValueT, typename... _ArgsT>
inline Result<std::decay_t<_ValueT>> emplaceSuccess(_ArgsT&&... args) {
  return Result<std::decay_t<_ValueT>>{result_success{},
                                       typename Result<_ValueT>::value_t{std::forward<_ArgsT>(args)...}};
}

inline Result<void> success() {
  return Result<void>{result_success{}};
}

}  // namespace Xi

#define XI_ERROR_TRY()        \
  using ::Xi::success;        \
  using ::Xi::emplaceSuccess; \
  using ::Xi::failure;        \
  try {                       \
    do {                      \
  } while (0)

#define XI_ERROR_CATCH()                              \
  }                                                   \
  catch (...) {                                       \
    return ::Xi::makeError(std::current_exception()); \
  }                                                   \
  do {                                                \
  } while (0)
