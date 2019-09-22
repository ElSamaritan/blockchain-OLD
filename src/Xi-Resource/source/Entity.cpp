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

#include "Xi/Resource/Entity.hpp"

#include <cassert>

#include <Xi/Exceptions.hpp>

namespace Xi {
namespace Resource {

Entity::Entity() : m_data{nullptr} {
  /* */
}

Type Entity::type() const {
  if (std::holds_alternative<embedded_binary_storage>(m_data)) {
    return Type::Binary;
  } else if (std::holds_alternative<embedded_text_storage>(m_data)) {
    return Type::Text;
  } else if (std::holds_alternative<null_storage>(m_data)) {
    exceptional<NotInitializedError>();
  } else {
    exceptional<InvalidVariantTypeError>();
  }
}

const ByteVector &Entity::binary() const {
  const auto bin = std::get_if<embedded_binary_storage>(std::addressof(m_data));
  exceptional_if<InvalidVariantTypeError>(bin == nullptr);
  assert(*bin != nullptr);
  return **bin;
}

const std::string &Entity::text() const {
  const auto text = std::get_if<embedded_text_storage>(std::addressof(m_data));
  exceptional_if<InvalidVariantTypeError>(text == nullptr);
  assert(*text != nullptr);
  return **text;
}

Entity::Entity(const ByteVector &binary) : m_data{std::addressof(binary)} {
  /* */
}

Entity::Entity(const std::string &text) : m_data{std::addressof(text)} {
  /* */
}

}  // namespace Resource
}  // namespace Xi
