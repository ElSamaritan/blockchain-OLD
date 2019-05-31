﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "ISerializer.h"

#include <array>
#include <cstring>
#include <list>
#include <map>
#include <string>
#include <set>
#include <type_traits>
#include <vector>
#include <map>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include <Xi/Global.hh>

#include <Xi/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <boost/utility/value_init.hpp>
#include <Xi/ExternalIncludePop.h>

#include "Serialization/ArraySerialization.hpp"
#include "Serialization/OptionalSerialization.hpp"
#include "Serialization/VariantSerialization.hpp"
#include "Serialization/EnumSerialization.hpp"
#include "Serialization/FlagSerialization.hpp"

namespace CryptoNote {

template <typename T>
[[nodiscard]] bool serializeAsBinary(std::vector<T>& value, Common::StringView name,
                                     CryptoNote::ISerializer& serializer) {
  static_assert(std::is_pod_v<T>, "only pods can be serialized as raw binaries");
  std::string blob;
  if (serializer.type() == ISerializer::INPUT) {
    XI_RETURN_EC_IF_NOT(serializer.binary(blob, name), false);
    value.resize(blob.size() / sizeof(T));
    if (blob.size()) {
      memcpy(&value[0], blob.data(), blob.size());
    }
    return true;
  } else {
    if (!value.empty()) {
      blob.assign(reinterpret_cast<const char*>(&value[0]), value.size() * sizeof(T));
    }
    XI_RETURN_EC_IF_NOT(serializer.binary(blob, name), false);
    return true;
  }
}

template <typename T>
[[nodiscard]] bool serializeAsBinary(std::list<T>& value, Common::StringView name,
                                     CryptoNote::ISerializer& serializer) {
  static_assert(std::is_pod_v<T>, "only pods can be serialized as raw binaries");
  std::string blob;
  if (serializer.type() == ISerializer::INPUT) {
    XI_RETURN_EC_IF_NOT(serializer.binary(blob, name), false);

    size_t count = blob.size() / sizeof(T);
    const T* ptr = reinterpret_cast<const T*>(blob.data());

    while (count--) {
      value.push_back(*ptr++);
    }
    return true;
  } else {
    if (!value.empty()) {
      blob.resize(value.size() * sizeof(T));
      T* ptr = reinterpret_cast<T*>(&blob[0]);

      for (const auto& item : value) {
        *ptr++ = item;
      }
    }
    XI_RETURN_EC_IF_NOT(serializer.binary(blob, name), false);
    return true;
  }
}

template <typename Cont>
[[nodiscard]] bool serializeContainer(Cont& value, Common::StringView name, CryptoNote::ISerializer& serializer) {
  size_t size = value.size();
  if (!serializer.beginArray(size, name)) {
    if (serializer.type() == ISerializer::INPUT) {
      value.clear();
    }

    return false;
  }

  value.resize(size);

  for (auto& item : value) {
    XI_RETURN_EC_IF_NOT(serializer(const_cast<typename Cont::value_type&>(item), ""), false);
  }

  serializer.endArray();
  return true;
}

template <typename T>
[[nodiscard]] bool serialize(std::vector<T>& value, Common::StringView name, CryptoNote::ISerializer& serializer) {
  return serializeContainer(value, name, serializer);
}

template <typename T>
[[nodiscard]] bool serialize(std::list<T>& value, Common::StringView name, CryptoNote::ISerializer& serializer) {
  return serializeContainer(value, name, serializer);
}

template <typename MapT, typename ReserveOp>
[[nodiscard]] bool serializeMap(MapT& value, Common::StringView name, CryptoNote::ISerializer& serializer,
                                ReserveOp reserve) {
  size_t size = value.size();

  if (!serializer.beginArray(size, name)) {
    if (serializer.type() == ISerializer::INPUT) {
      value.clear();
    }

    return false;
  }

  if (serializer.type() == CryptoNote::ISerializer::INPUT) {
    reserve(size);

    for (size_t i = 0; i < size; ++i) {
      typename MapT::key_type key;
      typename MapT::mapped_type v;

      XI_RETURN_EC_IF_NOT(serializer.beginObject(""), false);
      XI_RETURN_EC_IF_NOT(serializer(key, "key"), false);
      XI_RETURN_EC_IF_NOT(serializer(v, "value"), false);
      serializer.endObject();

      value.insert(std::make_pair(std::move(key), std::move(v)));
    }
  } else {
    for (auto& kv : value) {
      XI_RETURN_EC_IF_NOT(serializer.beginObject(""), false);
      XI_RETURN_EC_IF_NOT(serializer(const_cast<typename MapT::key_type&>(kv.first), "key"), false);
      XI_RETURN_EC_IF_NOT(serializer(kv.second, "value"), false);
      serializer.endObject();
    }
  }

  serializer.endArray();
  return true;
}

template <typename SetT>
[[nodiscard]] bool serializeSet(SetT& value, Common::StringView name, CryptoNote::ISerializer& serializer) {
  size_t size = value.size();

  if (!serializer.beginArray(size, name)) {
    if (serializer.type() == ISerializer::INPUT) {
      value.clear();
    }

    return false;
  }

  if (serializer.type() == CryptoNote::ISerializer::INPUT) {
    for (size_t i = 0; i < size; ++i) {
      typename SetT::value_type key;
      XI_RETURN_EC_IF_NOT(serializer(key, ""), false);
      value.insert(std::move(key));
    }
  } else {
    for (auto& key : value) {
      XI_RETURN_EC_IF_NOT(serializer(const_cast<typename SetT::value_type&>(key), ""), false);
    }
  }

  serializer.endArray();
  return true;
}

template <typename K, typename Hash>
[[nodiscard]] bool serialize(std::unordered_set<K, Hash>& value, Common::StringView name,
                             CryptoNote::ISerializer& serializer) {
  return serializeSet(value, name, serializer);
}

template <typename K, typename Cmp>
[[nodiscard]] bool serialize(std::set<K, Cmp>& value, Common::StringView name, CryptoNote::ISerializer& serializer) {
  return serializeSet(value, name, serializer);
}

template <typename K, typename V, typename Hash>
[[nodiscard]] bool serialize(std::unordered_map<K, V, Hash>& value, Common::StringView name,
                             CryptoNote::ISerializer& serializer) {
  return serializeMap(value, name, serializer, [&value](size_t size) { value.reserve(size); });
}

template <typename K, typename V, typename Hash>
[[nodiscard]] bool serialize(std::unordered_multimap<K, V, Hash>& value, Common::StringView name,
                             CryptoNote::ISerializer& serializer) {
  return serializeMap(value, name, serializer, [&value](size_t size) { value.reserve(size); });
}

template <typename K, typename V, typename Hash>
[[nodiscard]] bool serialize(std::map<K, V, Hash>& value, Common::StringView name,
                             CryptoNote::ISerializer& serializer) {
  return serializeMap(value, name, serializer, [](size_t size) { (void)size; });
}

template <typename K, typename V, typename Hash>
[[nodiscard]] bool serialize(std::multimap<K, V, Hash>& value, Common::StringView name,
                             CryptoNote::ISerializer& serializer) {
  return serializeMap(value, name, serializer, [](size_t size) { (void)size; });
}

template <typename T1, typename T2>
[[nodiscard]] bool serialize(std::pair<T1, T2>& value, ISerializer& s) {
  XI_RETURN_EC_IF_NOT(s(value.first, "key"), false);
  XI_RETURN_EC_IF_NOT(s(value.second, "value"), false);
  return true;
}

template <typename Element, typename Iterator>
[[nodiscard]] bool writeSequence(Iterator begin, Iterator end, Common::StringView name, ISerializer& s) {
  size_t size = std::distance(begin, end);
  XI_RETURN_EC_IF_NOT(s.beginArray(size, name), false);
  for (Iterator i = begin; i != end; ++i) {
    XI_RETURN_EC_IF_NOT(s(const_cast<Element&>(*i), ""), false);
  }
  s.endArray();
  return true;
}

template <typename Element, typename Iterator>
[[nodiscard]] bool readSequence(Iterator outputIterator, Common::StringView name, ISerializer& s) {
  size_t size = 0;
  // array of zero size is not written in KVBinaryOutputStreamSerializer
  if (!s.beginArray(size, name)) {
    return false;
  }

  while (size--) {
    Element e;
    XI_RETURN_EC_IF_NOT(s(e, ""), false);
    *outputIterator++ = std::move(e);
  }

  s.endArray();
  return true;
}

}  // namespace CryptoNote
