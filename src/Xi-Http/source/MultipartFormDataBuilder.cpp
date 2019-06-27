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

#include "Xi/Http/MultipartFormDataBuilder.h"

#include <fstream>
#include <utility>

#include <Xi/ExternalIncludePush.h>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Encoding/Base64.h>

namespace {
const std::string LF{"\r\n"};
const std::string NewLine{"\n"};

std::string makeDisposition(const std::string &name) {
  return std::string{"Content-Disposition: form-data; name=\""} + name + "\"";
}
std::string makeContentType(Xi::Http::ContentType type) { return std::string{"Content-Type: "} + Xi::to_string(type); }
}  // namespace

Xi::Http::MultipartFormDataBuilder::MultipartFormDataBuilder()
    : m_boundary{boost::lexical_cast<std::string>(boost::uuids::random_generator{}())}, m_body{NewLine} {}

Xi::Http::MultipartFormDataBuilder::~MultipartFormDataBuilder() {}

void Xi::Http::MultipartFormDataBuilder::addFile(const std::string &name, const std::string &filepath,
                                                 ContentType type) {
  std::ifstream file;
  if (type == ContentType::Binary)
    file = std::ifstream{filepath, std::ios::in | std::ios::binary};
  else
    file = std::ifstream{filepath, std::ios::in};
  const boost::filesystem::path p{filepath};
  m_body << "--" << m_boundary << LF << makeDisposition(name) << "; filename=\"" << p.filename() << "." << p.stem()
         << "\"" << LF << "Content-Type: " << makeContentType(Xi::Http::ContentType::Binary) << LF << LF;
  std::copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(),
            std::ostreambuf_iterator<char>(m_body));
  m_body << LF;
}

void Xi::Http::MultipartFormDataBuilder::addField(const std::string &name, const std::string &value) {
  m_body << "--" << m_boundary << LF << makeDisposition(name) << LF << LF << value << LF;
}

Xi::Http::Request Xi::Http::MultipartFormDataBuilder::request(const std::string &target, Method method) const {
  Request reval{target, method};
  reval.headers().set(HeaderContainer::ContentType,
                      Xi::to_string(ContentType::MultipartFormData) + "; boundary=" + m_boundary);
  std::string body = m_body.str() + "--" + m_boundary + "--" + LF;
  reval.setBody(std::move(body));
  return reval;
}
