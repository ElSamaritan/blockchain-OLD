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

#include <variant>
#include <cassert>

#include <Xi/Global.hh>

#include "Serialization/ISerializer.h"
#include "Serialization/TypeTag.hpp"

namespace CryptoNote {

namespace Impl {

template <typename _VariantT, size_t _Index>
inline TypeTag getVariantTypeTag();

template <size_t _Index, typename _InvariantT, typename... _Ts>
[[nodiscard]] inline bool serializeVariantInput(std::variant<_Ts...> &value, const TypeTag &tag,
                                                ISerializer &serializer) {
  assert(serializer.isInput());
  if constexpr (_Index == sizeof...(_Ts)) {
    XI_UNUSED(value, tag, serializer);
    return false;
  } else {
    using native_t = std::variant_alternative_t<_Index, std::variant<_Ts...>>;
    if (tag == getVariantTypeTag<_InvariantT, _Index>()) {
      native_t nativeValue;
      XI_RETURN_EC_IF_NOT(serializer(nativeValue, "value"), false);
      value = std::move(nativeValue);
      return true;
    } else {
      return serializeVariantInput<_Index + 1, _InvariantT, _Ts...>(value, tag, serializer);
    }
  }
}

template <size_t _Index, typename _InvariantT, typename... _Ts>
[[nodiscard]] inline bool serializeVariantOutput(std::variant<_Ts...> &value, ISerializer &serializer) {
  assert(serializer.isOutput());
  if constexpr (_Index == sizeof...(_Ts)) {
    XI_UNUSED(value, serializer);
    return false;
  } else {
    if (value.index() == _Index) {
      auto &varValue = std::get<_Index>(value);
      TypeTag tag{getVariantTypeTag<_InvariantT, _Index>()};
      XI_RETURN_EC_IF_NOT(serializer.typeTag(tag, "type"), false);
      XI_RETURN_EC_IF_NOT(serializer(varValue, "value"), false);
      return true;
    } else {
      return serializeVariantOutput<_Index + 1, _InvariantT, _Ts...>(value, serializer);
    }
  }
}

}  // namespace Impl

template <typename _InvariantT, typename... _Ts>
[[nodiscard]] inline bool serializeVariant(std::variant<_Ts...> &value, Common::StringView name,
                                           ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer.beginObject(name), false);
  if (serializer.isInput()) {
    TypeTag tag{TypeTag::NoBinaryTag, TypeTag::NoTextTag};
    XI_RETURN_EC_IF_NOT(serializer.typeTag(tag, "type"), false);
    XI_RETURN_EC_IF(tag.isNull(), false);
    if (!CryptoNote::Impl::serializeVariantInput<0, _InvariantT, _Ts...>(value, tag, serializer)) {
      return false;
    }
  } else {
    assert(serializer.isOutput());
    if (!CryptoNote::Impl::serializeVariantOutput<0, _InvariantT, _Ts...>(value, serializer)) {
      return false;
    }
  }
  XI_RETURN_EC_IF_NOT(serializer.endObject(), false);
  return true;
}

template <typename... _Ts>
[[nodiscard]] inline bool serialize(std::variant<_Ts...> &value, Common::StringView name, ISerializer &serializer) {
  return serializeVariant<std::variant<_Ts...>>(value, name, serializer);
}

}  // namespace CryptoNote

#define XI_SERIALIZATION_VARIANT_TAG(VARIANT_TYPE, VALUE_INDEX, BINARY, TEXT) \
  namespace CryptoNote {                                                      \
  namespace Impl {                                                            \
  template <>                                                                 \
  inline TypeTag getVariantTypeTag<VARIANT_TYPE, VALUE_INDEX>() {             \
    return TypeTag{BINARY, TEXT};                                             \
  }                                                                           \
  }                                                                           \
  }

#define XI_SERIALIZATION_VARIANT_INVARIANT(VARIANT_TYPE, ...)                                                  \
  struct VARIANT_TYPE : std::variant<__VA_ARGS__> {                                                            \
    using variant::variant;                                                                                    \
  };                                                                                                           \
  [[nodiscard]] inline bool serialize(VARIANT_TYPE &value, ::Common::StringView name,                          \
                                      ::CryptoNote::ISerializer &serializer) {                                 \
    return ::CryptoNote::serializeVariant<VARIANT_TYPE>(static_cast<std::variant<__VA_ARGS__> &>(value), name, \
                                                        serializer);                                           \
  }
