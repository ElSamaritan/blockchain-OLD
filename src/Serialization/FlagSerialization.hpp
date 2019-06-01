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

template <typename _FlagT>
struct FlagRange {
  static inline constexpr uint16_t begin() { return 0; }
  static inline constexpr uint16_t end() { return 0; }
  static inline constexpr uint8_t beginShift() { return 0; }
  static inline constexpr uint8_t endShift() { return 0; }
};

template <uint16_t _IndexV, uint8_t _CurrentV = 0>
inline constexpr uint8_t getFlagShift() {
  static_assert(_IndexV > 0, "0 == (1 << X) has no solution.");
  static_assert(_CurrentV < 14, "only 14 bits ares upported for flags.");
  if constexpr ((_IndexV & (1 << _CurrentV)) > 0) {
    static_assert((_IndexV & (1 << _CurrentV)) == _IndexV, "fags may have exactly one bit set.");
    return _CurrentV;
  } else {
    return getFlagShift<_IndexV, _CurrentV + 1>();
  }
}

template <typename _FlagT, uint16_t _IndexV>
TypeTag getFlagTypeTag();

template <typename _FlagT, uint16_t _IndexV>
inline constexpr bool hasFlagTypeTag() {
  return false;
}

template <typename _FlagT, uint8_t _ShiftV = FlagRange<_FlagT>::beginShift()>
[[nodiscard]] bool getFlag(_FlagT& flag, const TypeTag& tag) {
  if constexpr (_ShiftV > FlagRange<_FlagT>::endShift()) {
    XI_UNUSED(flag, tag);
    return false;
  } else {
    if constexpr (hasFlagTypeTag<_FlagT, (1 << _ShiftV)>()) {
      if (tag == getFlagTypeTag<_FlagT, (1 << _ShiftV)>()) {
        flag = static_cast<_FlagT>(1 << _ShiftV);
        return true;
      }
    }
    return getFlag<_FlagT, _ShiftV + 1>(flag, tag);
  }
}

template <typename _FlagT, uint8_t _ShiftV = FlagRange<_FlagT>::beginShift()>
[[nodiscard]] bool collectFlatTypeTags(TypeTagVector& tags, const _FlagT flag) {
  if constexpr (_ShiftV > FlagRange<_FlagT>::endShift()) {
    XI_UNUSED(tags, flag);
    return static_cast<uint16_t>(flag) < (1 << _ShiftV);
  } else {
    if ((static_cast<uint16_t>(flag) & (1 << _ShiftV)) > 0) {
      if constexpr (!hasFlagTypeTag<_FlagT, (1 << _ShiftV)>()) {
        return false;
      } else {
        tags.push_back(getFlagTypeTag<_FlagT, (1 << _ShiftV)>());
      }
    }
    return collectFlatTypeTags<_FlagT, _ShiftV + 1U>(tags, flag);
  }
}

template <typename _FlagT>
[[nodiscard]] inline bool serializeFlag(_FlagT& value, Common::StringView name, ISerializer& serializer) {
  if (serializer.isInput()) {
    TypeTagVector tags;
    XI_RETURN_EC_IF_NOT(serializer.flag(tags, name), false);
    _FlagT composition = static_cast<_FlagT>(0);
    for (const auto& tag : tags) {
      _FlagT iFlag = static_cast<_FlagT>(0);
      XI_RETURN_EC_IF_NOT(getFlag<_FlagT>(iFlag, tag), false);
      XI_RETURN_EC_IF(iFlag == static_cast<_FlagT>(0), false);
      composition = composition | iFlag;
    }
    value = composition;
    return true;
  } else {
    assert(serializer.isOutput());
    TypeTagVector tags;
    XI_RETURN_EC_IF_NOT(collectFlatTypeTags<_FlagT>(tags, value), false);
    return serializer.flag(tags, name);
  }
}

}  // namespace Impl

}  // namespace CryptoNote

#define XI_SERIALIZATION_FLAG(FLAG_TYPE)                                           \
  [[nodiscard]] inline bool serialize(FLAG_TYPE& value, ::Common::StringView name, \
                                      ::CryptoNote::ISerializer& serializer) {     \
    return ::CryptoNote::Impl::serializeFlag<FLAG_TYPE>(value, name, serializer);  \
  }

#define XI_SERIALIZATION_FLAG_RANGE(FLAG_TYPE, FLAG_BEGIN, FLAG_END)                                                  \
  namespace CryptoNote {                                                                                              \
  namespace Impl {                                                                                                    \
  template <>                                                                                                         \
  struct FlagRange<FLAG_TYPE> {                                                                                       \
    static inline constexpr uint16_t begin() { return static_cast<uint16_t>(FLAG_TYPE::FLAG_BEGIN); }                 \
    static inline constexpr uint16_t end() { return static_cast<uint16_t>(FLAG_TYPE::FLAG_END); }                     \
    static inline constexpr uint8_t beginShift() {                                                                    \
      return getFlagShift<static_cast<uint16_t>(FLAG_TYPE::FLAG_BEGIN)>();                                            \
    }                                                                                                                 \
    static inline constexpr uint8_t endShift() { return getFlagShift<static_cast<uint16_t>(FLAG_TYPE::FLAG_END)>(); } \
  };                                                                                                                  \
  }                                                                                                                   \
  }

#define XI_SERIALIZATION_FLAG_TAG(FLAG_TYPE, FLAG_VALUE, TEXT)                                      \
  namespace CryptoNote {                                                                            \
  namespace Impl {                                                                                  \
  template <>                                                                                       \
  inline constexpr bool hasFlagTypeTag<FLAG_TYPE, static_cast<uint16_t>(FLAG_TYPE::FLAG_VALUE)>() { \
    return true;                                                                                    \
  }                                                                                                 \
  template <>                                                                                       \
  inline TypeTag getFlagTypeTag<FLAG_TYPE, static_cast<uint16_t>(FLAG_TYPE::FLAG_VALUE)>() {        \
    return TypeTag{getFlagShift<static_cast<uint16_t>(FLAG_TYPE::FLAG_VALUE)>() + 1U, TEXT};        \
  }                                                                                                 \
  }                                                                                                 \
  }
