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

#include <type_traits>
#include <cassert>
#include <cinttypes>

#include "Serialization/ISerializer.h"
#include "Serialization/TypeTag.hpp"

namespace CryptoNote {

namespace Impl {

template <typename _EnumT>
struct EnumRange {
  static inline constexpr uint8_t begin() { return 0; }
  static inline constexpr uint8_t end() { return 0; }
};

template <typename _EnumT, uint8_t _IndexV>
TypeTag getEnumTypeTag();

template <typename _EnumT, uint8_t _IndexV>
inline constexpr bool hasEnumTypeTag() {
  return false;
}

template <typename _EnumT, uint8_t _IndexV>
[[nodiscard]] inline bool serializeIndexedEnumInput(_EnumT& value, Common::StringView name, const TypeTag& tag) {
  using Range = EnumRange<_EnumT>;
  if constexpr (_IndexV > Range::end()) {
    XI_UNUSED(value, name, tag);
    return false;
  } else if constexpr (!hasEnumTypeTag<_EnumT, _IndexV>()) {
    return serializeIndexedEnumInput<_EnumT, _IndexV + 1>(value, name, tag);
  } else {
    if (tag == getEnumTypeTag<_EnumT, _IndexV>()) {
      value = static_cast<_EnumT>(_IndexV);
      return true;
    } else {
      return serializeIndexedEnumInput<_EnumT, _IndexV + 1>(value, name, tag);
    }
  }
}

template <typename _EnumT, uint8_t _IndexV>
[[nodiscard]] inline bool serializeIndexedEnumOutput(_EnumT& value, Common::StringView name, ISerializer& serializer) {
  using Range = EnumRange<_EnumT>;
  if constexpr (_IndexV > Range::end()) {
    XI_UNUSED(value, name, serializer);
    return false;
  } else {
    if (value == static_cast<_EnumT>(_IndexV)) {
      if constexpr (!hasEnumTypeTag<_EnumT, _IndexV>()) {
        return false;
      } else {
        TypeTag tag = getEnumTypeTag<_EnumT, _IndexV>();
        XI_RETURN_EC_IF(tag.isNull(), false);
        XI_RETURN_EC_IF_NOT(serializer.typeTag(tag, name), false);
        return true;
      }
    } else {
      return serializeIndexedEnumOutput<_EnumT, _IndexV + 1>(value, name, serializer);
    }
  }
}

template <typename _EnumT>
[[nodiscard]] inline bool serializeEnum(_EnumT& value, Common::StringView name, ISerializer& serializer) {
  if (serializer.isInput()) {
    TypeTag tag = TypeTag::Null;
    XI_RETURN_EC_IF_NOT(serializer.typeTag(tag, name), false);
    XI_RETURN_EC_IF(tag.isNull(), false);
    return serializeIndexedEnumInput<_EnumT, EnumRange<_EnumT>::begin()>(value, name, tag);
  } else {
    return serializeIndexedEnumOutput<_EnumT, EnumRange<_EnumT>::begin()>(value, name, serializer);
  }
}

}  // namespace Impl

}  // namespace CryptoNote

#define XI_SERIALIZATION_ENUM(ENUM_TYPE)                                           \
  [[nodiscard]] inline bool serialize(ENUM_TYPE& value, ::Common::StringView name, \
                                      ::CryptoNote::ISerializer& serializer) {     \
    return ::CryptoNote::Impl::serializeEnum<ENUM_TYPE>(value, name, serializer);  \
  }

#define XI_SERIALIZATION_ENUM_RANGE(ENUM_TYPE, ENUM_BEGIN, ENUM_END)                                \
  namespace CryptoNote {                                                                            \
  namespace Impl {                                                                                  \
  template <>                                                                                       \
  struct EnumRange<ENUM_TYPE> {                                                                     \
    static inline constexpr uint8_t begin() { return static_cast<uint8_t>(ENUM_TYPE::ENUM_BEGIN); } \
    static inline constexpr uint8_t end() { return static_cast<uint8_t>(ENUM_TYPE::ENUM_END); }     \
    static_assert(0 < static_cast<uint8_t>(ENUM_TYPE::ENUM_BEGIN) &&                                \
                      static_cast<uint8_t>(ENUM_TYPE::ENUM_END) < 0b01111111,                       \
                  "invalid enum range");                                                            \
  };                                                                                                \
  }                                                                                                 \
  }

#define XI_SERIALIZATION_ENUM_TAG(ENUM_TYPE, ENUM_VALUE, TEXT)                                     \
  namespace CryptoNote {                                                                           \
  namespace Impl {                                                                                 \
  template <>                                                                                      \
  inline constexpr bool hasEnumTypeTag<ENUM_TYPE, static_cast<uint8_t>(ENUM_TYPE::ENUM_VALUE)>() { \
    return true;                                                                                   \
  }                                                                                                \
  template <>                                                                                      \
  inline TypeTag getEnumTypeTag<ENUM_TYPE, static_cast<uint8_t>(ENUM_TYPE::ENUM_VALUE)>() {        \
    return TypeTag{static_cast<uint8_t>(ENUM_TYPE::ENUM_VALUE), TEXT};                             \
  }                                                                                                \
  }                                                                                                \
  }
