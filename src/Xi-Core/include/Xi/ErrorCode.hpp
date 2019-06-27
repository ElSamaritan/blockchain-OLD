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

#include <cinttypes>
#include <string>
#include <system_error>
#include <type_traits>

#include <fmt/format.h>

#include "Xi/Error.h"

#define XI_ERROR_CODE_BEGIN(NAME)                   \
  enum struct [[nodiscard]] NAME##Error : uint8_t { \
    Success = 0x0000,
#define XI_ERROR_CODE_VALUE(NAME, VALUE) NAME = VALUE,
#define XI_ERROR_CODE_END(NAME, DESC)                                             \
  }                                                                               \
  ;                                                                               \
  struct NAME##ErrorCategory final : public std::error_category {                 \
    const char *name() const noexcept override { return #DESC; }                  \
    std::string message(int ev) const override;                                   \
                                                                                  \
   private:                                                                       \
    NAME##ErrorCategory() = default;                                              \
                                                                                  \
   public:                                                                        \
    static NAME##ErrorCategory Instance;                                          \
  };                                                                              \
  inline std::error_code make_error_code(NAME##Error err) {                       \
    return std::error_code(static_cast<int>(err), NAME##ErrorCategory::Instance); \
  }                                                                               \
  inline std::string toString(NAME##Error err) {                                  \
    return NAME##ErrorCategory::Instance.message(static_cast<int>(err));          \
  }                                                                               \
  inline ::Xi::Error makeError(NAME##Error ec) { return ::Xi::makeError(make_error_code(ec)); }

#define XI_ERROR_CODE_CATEGORY_BEGIN(NAMESPACE, NAME)                        \
  NAMESPACE::NAME##ErrorCategory NAMESPACE::NAME##ErrorCategory::Instance{}; \
  std::string NAMESPACE::NAME##ErrorCategory::message(int ev) const {        \
    using error_code_type = NAME##Error;                                     \
    const NAME##Error code = static_cast<error_code_type>(ev);               \
    switch (code) {                                                          \
      case error_code_type::Success:                                         \
        return "success";

#define XI_ERROR_CODE_DESC(VALUE, DESC) \
  case error_code_type::VALUE:          \
    return DESC;

#define XI_ERROR_CODE_CATEGORY_END() \
  }                                  \
  return "unknown";                  \
  }

#define XI_ERROR_CODE_OVERLOADS(NAMESPACE, NAME)                                           \
  namespace std {                                                                          \
  template <>                                                                              \
  struct is_error_code_enum<NAMESPACE::NAME##Error> : public true_type {};                 \
  inline std::error_code make_error_code(NAMESPACE::NAME##Error e) {                       \
    return std::error_code(static_cast<int>(e), NAMESPACE::NAME##ErrorCategory::Instance); \
  }                                                                                        \
  }                                                                                        \
  namespace fmt {                                                                          \
  template <>                                                                              \
  struct formatter<NAMESPACE::NAME##Error> {                                               \
    template <typename ParseContext>                                                       \
    constexpr auto parse(ParseContext &ctx) {                                              \
      return ctx.begin();                                                                  \
    }                                                                                      \
                                                                                           \
    template <typename FormatContext>                                                      \
    auto format(const NAMESPACE::NAME##Error &err, FormatContext &ctx) {                   \
      return format_to(ctx.begin(), toString(err));                                        \
    }                                                                                      \
  };                                                                                       \
  }  // namespace fmt
