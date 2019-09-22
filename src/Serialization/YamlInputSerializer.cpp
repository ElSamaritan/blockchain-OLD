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

#include "Serialization/YamlInputSerializer.hpp"

#include <stack>
#include <optional>
#include <utility>
#include <limits>
#include <cinttypes>
#include <unordered_set>

#include <Xi/ExternalIncludePush.h>
#include <yaml-cpp/yaml.h>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Algorithm/String.h>
#include <Common/StringTools.h>

#define XI_YAML_TRY try {
#define XI_YAML_CATCH                        \
  }                                          \
  catch (const std::exception &e) {          \
    XI_UNUSED(e)                             \
    XI_PRINT_EC("YAML threw: %s", e.what()); \
    XI_RETURN_EC(false);                     \
  }                                          \
  catch (...) {                              \
    XI_PRINT_EC("YAML threw: UNKNOWN");      \
    XI_RETURN_EC(false);                     \
  }

namespace CryptoNote {

struct YamlInputSerializer::_Impl {
  std::optional<YAML::Node> root{std::nullopt};
  std::stack<YAML::Node> nodes;
  std::stack<uint32_t> indices;

  const YAML::Node *top() {
    XI_RETURN_SC_IF(nodes.empty(), nullptr);
    XI_RETURN_SC(std::addressof(nodes.top()));
  }

  [[nodiscard]] bool push(YAML::Node node) {
    XI_RETURN_EC_IF_NOT(node.IsMap() || node.IsSequence(), false);
    nodes.push(std::move(node));
    indices.push(0);
    XI_RETURN_SC(true);
  }

  [[nodiscard]] bool pop() {
    XI_RETURN_EC_IF(nodes.empty(), false);
    XI_RETURN_EC_IF(indices.empty(), false);
    nodes.pop();
    indices.pop();
    XI_RETURN_SC(true);
  }

  [[nodiscard]] std::optional<YAML::Node> child(Common::StringView name) {
    auto current = top();
    if (current == nullptr) {
      XI_RETURN_EC_IF_NOT(root, std::nullopt);
      auto _root = root;
      root = std::nullopt;
      XI_RETURN_SC(_root);
    } else {
      if (current->IsMap()) {
        YAML::Node value = (*current)[std::string{name.getData(), name.getSize()}];
        XI_RETURN_EC_IF_NOT(value, std::nullopt);
        XI_RETURN_SC(std::move(value));
      } else if (current->IsSequence()) {
        const auto index = indices.top();
        XI_RETURN_EC_IF_NOT(index < current->size(), std::nullopt);
        auto value = (*current)[index];
        XI_RETURN_EC_IF_NOT(value, std::nullopt);
        indices.top() += 1;
        XI_RETURN_SC(std::move(value));
      } else {
        XI_RETURN_EC(std::nullopt);
      }
    }
  }

  [[nodiscard]] bool child(Common::StringView name, std::string &out) {
    XI_YAML_TRY
    auto value = child(name);
    XI_RETURN_EC_IF_NOT(value, false);
    XI_RETURN_EC_IF_NOT(value->IsScalar(), false);
    out = value->Scalar();
    XI_RETURN_SC(true);
    XI_YAML_CATCH
  }

  template <typename _IntegerT, typename = std::enable_if_t<std::is_integral_v<_IntegerT>>>
  [[nodiscard]] bool child(Common::StringView name, _IntegerT &out) {
    using integer64_t = std::conditional_t<std::is_signed_v<_IntegerT>, int64_t, uint64_t>;

    XI_YAML_TRY
    std::string scalar{};
    XI_RETURN_EC_IF_NOT(child(name, scalar), false);
    scalar = Xi::trim(scalar);

    integer64_t number = 0;
    if constexpr (std::is_signed_v<_IntegerT>) {
      number = std::stoll(scalar);
    } else {
      number = std::stoull(scalar);
    }

    if constexpr (std::numeric_limits<_IntegerT>::max() < std::numeric_limits<integer64_t>::max()) {
      XI_RETURN_EC_IF(number > std::numeric_limits<_IntegerT>::max(), false);
    }
    if constexpr (std::numeric_limits<_IntegerT>::min() > std::numeric_limits<integer64_t>::min()) {
      XI_RETURN_EC_IF(number < std::numeric_limits<_IntegerT>::min(), false);
    }

    out = static_cast<_IntegerT>(number);
    XI_RETURN_SC(true);

    XI_YAML_CATCH
  }
};

std::unique_ptr<YamlInputSerializer> YamlInputSerializer::parse(const std::string &content) {
  try {
    auto node = YAML::Load(content);
    std::unique_ptr<YamlInputSerializer> reval{new YamlInputSerializer};
    reval->m_impl->root = node;
    XI_RETURN_SC(reval);
  } catch (const std::exception &e) {
    XI_UNUSED(e)
    XI_PRINT_EC("YAML parse thre: %s", e.what());
    XI_RETURN_EC(nullptr);
  } catch (...) {
    XI_PRINT_EC("YAML parse thre: UNKNOWN");
    XI_RETURN_EC(nullptr);
  }
}

YamlInputSerializer::~YamlInputSerializer() {
  /* */
}

ISerializer::SerializerType YamlInputSerializer::type() const {
  return INPUT;
}

ISerializer::FormatType YamlInputSerializer::format() const {
  return HumanReadable;
}

bool YamlInputSerializer::beginObject(Common::StringView name) {
  XI_YAML_TRY
  auto value = m_impl->child(name);
  XI_RETURN_EC_IF_NOT(value, false);
  XI_RETURN_EC_IF_NOT(value->IsMap(), false);
  XI_RETURN_EC_IF_NOT(m_impl->push(*value), false);
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::endObject() {
  XI_YAML_TRY
  auto top = m_impl->top();
  XI_RETURN_EC_IF_NOT(top != nullptr, false);
  XI_RETURN_EC_IF_NOT(top->IsMap(), false);
  XI_RETURN_EC_IF_NOT(m_impl->pop(), false);
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::beginArray(size_t &size, Common::StringView name) {
  XI_YAML_TRY
  auto value = m_impl->child(name);
  XI_RETURN_EC_IF_NOT(value, false);
  XI_RETURN_EC_IF_NOT(value->IsSequence(), false);
  size = value->size();
  XI_RETURN_EC_IF_NOT(m_impl->push(*value), false);
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::beginStaticArray(const size_t size, Common::StringView name) {
  XI_YAML_TRY
  auto value = m_impl->child(name);
  XI_RETURN_EC_IF_NOT(value, false);
  XI_RETURN_EC_IF_NOT(value->IsSequence(), false);
  XI_RETURN_EC_IF_NOT(value->size() == size, false);
  XI_RETURN_EC_IF_NOT(m_impl->push(*value), false);
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::endArray() {
  XI_YAML_TRY
  auto top = m_impl->top();
  XI_RETURN_EC_IF_NOT(top != nullptr, false);
  XI_RETURN_EC_IF_NOT(top->IsSequence(), false);
  XI_RETURN_EC_IF_NOT(m_impl->pop(), false);
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::operator()(uint8_t &value, Common::StringView name) {
  XI_YAML_TRY
  XI_RETURN_EC_IF_NOT(m_impl->child(name, value), false);
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::operator()(uint16_t &value, Common::StringView name) {
  XI_YAML_TRY
  XI_RETURN_EC_IF_NOT(m_impl->child(name, value), false);
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::operator()(int16_t &value, Common::StringView name) {
  XI_YAML_TRY
  XI_RETURN_EC_IF_NOT(m_impl->child(name, value), false);
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::operator()(uint32_t &value, Common::StringView name) {
  XI_YAML_TRY
  XI_RETURN_EC_IF_NOT(m_impl->child(name, value), false);
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::operator()(int32_t &value, Common::StringView name) {
  XI_YAML_TRY
  XI_RETURN_EC_IF_NOT(m_impl->child(name, value), false);
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::operator()(uint64_t &value, Common::StringView name) {
  XI_YAML_TRY
  XI_RETURN_EC_IF_NOT(m_impl->child(name, value), false);
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::operator()(int64_t &value, Common::StringView name) {
  XI_YAML_TRY
  XI_RETURN_EC_IF_NOT(m_impl->child(name, value), false);
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::operator()(std::string &value, Common::StringView name) {
  XI_YAML_TRY
  XI_RETURN_EC_IF_NOT(m_impl->child(name, value), false);
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::operator()(double &value, Common::StringView name) {
  XI_YAML_TRY
  auto node = m_impl->child(name);
  XI_RETURN_EC_IF_NOT(node, false);
  value = node->as<double>();
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::operator()(bool &value, Common::StringView name) {
  XI_YAML_TRY
  auto node = m_impl->child(name);
  XI_RETURN_EC_IF_NOT(node, false);
  value = node->as<bool>();
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::binary(void *value, size_t size, Common::StringView name) {
  XI_YAML_TRY

  Xi::ByteVector blob{};
  XI_RETURN_EC_IF_NOT(binary(blob, name), false);
  XI_RETURN_EC_IF_NOT(blob.size() == size, false);
  std::memcpy(value, blob.data(), size);
  XI_RETURN_SC(true);

  XI_YAML_CATCH
}

bool YamlInputSerializer::binary(std::string &value, Common::StringView name) {
  XI_YAML_TRY

  Xi::ByteVector blob{};
  XI_RETURN_EC_IF_NOT(binary(blob, name), false);
  value.resize(blob.size());
  std::memcpy(value.data(), blob.data(), blob.size());
  XI_RETURN_SC(true);

  XI_YAML_CATCH
}

bool YamlInputSerializer::binary(Xi::ByteVector &value, Common::StringView name) {
  XI_YAML_TRY

  std::string hex{};
  XI_RETURN_EC_IF_NOT(m_impl->child(name, hex), false);
  hex = Xi::trim(hex);
  value.clear();
  XI_RETURN_EC_IF_NOT(Common::fromHex(hex, value), false);
  XI_RETURN_SC(true);

  XI_YAML_CATCH
}

bool YamlInputSerializer::maybe(bool &value, Common::StringView name) {
  XI_YAML_TRY
  auto node = m_impl->child(name);
  if (!node) {
    value = false;
    XI_RETURN_SC(true);
  } else {
    value = node->as<bool>();
    XI_RETURN_SC(true);
  }
  XI_YAML_CATCH
}

bool YamlInputSerializer::typeTag(TypeTag &tag, Common::StringView name) {
  XI_YAML_TRY
  TypeTag::text_type tTag{TypeTag::NoTextTag};
  XI_RETURN_EC_IF_NOT(this->operator()(tTag, name), false);
  XI_RETURN_EC_IF(tTag == TypeTag::NoTextTag, false);
  tag = TypeTag{TypeTag::NoBinaryTag, tTag};
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

bool YamlInputSerializer::flag(std::vector<TypeTag> &flag, Common::StringView name) {
  XI_YAML_TRY
  flag.clear();
  bool hasFlag = false;
  XI_RETURN_EC_IF_NOT(maybe(hasFlag, name), false);
  XI_RETURN_SC_IF_NOT(hasFlag, true);

  size_t size = 0;
  XI_RETURN_EC_IF_NOT(beginArray(size, name), false);
  XI_RETURN_EC_IF(size > 14, false);

  std::unordered_set<std::string> processedFlags{};
  for (size_t i = 0; i < size; ++i) {
    TypeTag iTag = TypeTag::Null;
    XI_RETURN_EC_IF_NOT(typeTag(iTag, ""), false);
    XI_RETURN_EC_IF_NOT(processedFlags.insert(iTag.text()).second, false);
    flag.push_back(iTag);
  }
  XI_RETURN_EC_IF_NOT(endArray(), false);
  XI_RETURN_SC(true);
  XI_YAML_CATCH
}

#undef XI_YAML_TRY
#undef XI_YAML_CATCH

YamlInputSerializer::YamlInputSerializer() : m_impl{new _Impl} {
  /* */
}

}  // namespace CryptoNote
