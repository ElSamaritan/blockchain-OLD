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

#if defined(__cplusplus)
extern "C" {
#endif

#include <inttypes.h>
#include <stdlib.h>

#if !defined(NDEBUG)
#include <stdio.h>
#define XI_PRINT_EC(...) \
  printf(__VA_ARGS__);   \
  fflush(stdout)
#else
#define XI_PRINT_EC(...)
#endif

#define XI_TRUE 1
#define XI_FALSE 0

#define XI_RETURN_CODE_NO_SUCCESS 1
#define XI_RETURN_CODE_SUCCESS 0

#define XI_RETURN_EC(EC)                                                 \
  do {                                                                   \
    XI_PRINT_EC("[%s:%i] returns error: %s\n", __FILE__, __LINE__, #EC); \
    return EC;                                                           \
  } while (XI_FALSE)

#define XI_RETURN_EC_IF(COND, EC)                                                                         \
  do {                                                                                                    \
    if (COND) {                                                                                           \
      XI_PRINT_EC("[%s:%i] condition (%s) failed returning error: %s\n", __FILE__, __LINE__, #COND, #EC); \
      return EC;                                                                                          \
    }                                                                                                     \
  } while (XI_FALSE)

#define XI_RETURN_EC_IF_NOT(COND, EC)                                                                        \
  do {                                                                                                       \
    if (!(COND)) {                                                                                           \
      XI_PRINT_EC("[%s:%i] condition (!(%s)) failed returning error: %s\n", __FILE__, __LINE__, #COND, #EC); \
      return EC;                                                                                             \
    }                                                                                                        \
  } while (XI_FALSE)

#define XI_RETURN_SC(SC) \
  do {                   \
    return SC;           \
  } while (XI_FALSE)

#define XI_RETURN_SC_IF(COND, SC) \
  do {                            \
    if (COND) {                   \
      return SC;                  \
    }                             \
  } while (XI_FALSE)

#define XI_RETURN_SC_IF_NOT(COND, SC) \
  do {                                \
    if (!(COND)) {                    \
      return SC;                      \
    }                                 \
  } while (XI_FALSE)

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

#include <array>
#include <memory>

namespace Xi {
/*!
 * \brief Unreferenced marks the arguments as intentionally unreferenced.
 */
template <typename... Ts>
inline void Unreferenced(Ts&&...) {}

struct Null {};
constexpr Null null = Null{};

}  // namespace Xi

/*!
 * \def XI_UNUSED Marks a variable unused intentionally.
 */
#define XI_UNUSED(...) \
  { (decltype(Xi::Unreferenced(__VA_ARGS__)))0; }

/*!
 * \def XI_UNUSED_REVAL Marks a return value as intentionally unused. Mainly used for RAII objects.
 */
#define XI_UNUSED_REVAL(X) [[maybe_unused]] auto __UNUSED_REVAL_##__LINE__ = X;

/*!
 * \def XI_DELETE_COPY(CLASS_NAME) deletes any possible default generated copy constructor/assignment for CLASS_NAME
 */
#define XI_DELETE_COPY(CLASS_NAME)        \
  CLASS_NAME(const CLASS_NAME&) = delete; \
  CLASS_NAME& operator=(const CLASS_NAME&) = delete

/*!
 * \def XI_DEFAULT_COPY(CLASS_NAME) implements a default copy constructor/assignment for CLASS_NAME
 */
#define XI_DEFAULT_COPY(CLASS_NAME)        \
  CLASS_NAME(const CLASS_NAME&) = default; \
  CLASS_NAME& operator=(const CLASS_NAME&) = default

/*!
 * \def XI_DELETE_MOVE(CLASS_NAME) deletes any possible default generated move constructor/assignment for CLASS_NAME
 */
#define XI_DELETE_MOVE(CLASS_NAME)   \
  CLASS_NAME(CLASS_NAME&&) = delete; \
  CLASS_NAME& operator=(CLASS_NAME&&) = delete

/*!
 * \def XI_DEFAULT_MOVE(CLASS_NAME) implements a default move constructor/assignment for CLASS_NAME
 */
#define XI_DEFAULT_MOVE(CLASS_NAME)   \
  CLASS_NAME(CLASS_NAME&&) = default; \
  CLASS_NAME& operator=(CLASS_NAME&&) = default

#define XI_DECLARE_SMART_POINTER(TYPE)                   \
  using Unique##TYPE = std::unique_ptr<TYPE>;            \
  using Shared##TYPE = std::shared_ptr<TYPE>;            \
  using Weak##TYPE = std::weak_ptr<TYPE>;                \
  using SharedConst##TYPE = std::shared_ptr<const TYPE>; \
  using WeakConst##TYPE = std::weak_ptr<const TYPE>;

#define XI_DECLARE_SMART_POINTER_CLASS(TYPE) \
  class TYPE;                                \
  XI_DECLARE_SMART_POINTER(TYPE)

#define XI_DECLARE_SMART_POINTER_STRUCT(TYPE) \
  struct TYPE;                                \
  XI_DECLARE_SMART_POINTER(TYPE)

#define XI_PROPERTY(TYPE, NAME, ...)                         \
 private:                                                    \
  TYPE m_##NAME{__VA_ARGS__};                                \
                                                             \
 public:                                                     \
  inline const TYPE& NAME() const { return this->m_##NAME; } \
  inline TYPE& NAME() { return this->m_##NAME; }             \
  inline auto& NAME(const TYPE& _) {                         \
    this->m_##NAME = _;                                      \
    return *this;                                            \
  }

#define XI_PADDING(BYTES) std::array<unsigned char, BYTES> _padding;

#define may_throw
#define not_threadsafe
#define threadsafe

#endif
