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

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Error.h>

namespace Xi {
template <typename _ValueT>
class Result {
 public:
  using value_t = typename std::decay<_ValueT>::type;

 private:
  boost::variant<Error, value_t> m_result;

  const value_t& value(std::true_type) const { return boost::get<value_t>(m_result); }
  value_t value(std::false_type) const { return boost::get<value_t>(m_result); }

 public:
  /* implicit */ Result(const Error& err) : m_result{err} {}
  /* implicit */ Result(value_t&& value) : m_result{std::forward<value_t>(value)} {}

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

template <>
class Result<void> {
 private:
  boost::optional<Error> m_error;

 public:
  explicit Result() = default;
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
  return Result<void>{};
}

}  // namespace Xi
