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

#include "Xi/Network/Uri.hpp"

#include <utility>

#include <Xi/ExternalIncludePush.h>
#include <uriparser/Uri.h>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Exceptions.hpp>
#include <Xi/Algorithm/String.h>

namespace {
std::string toString(const UriTextRangeA &range) {
  return std::string{range.first, range.afterLast};
}

std::string toString(UriPathSegmentA *xs, const std::string &delim) {
  UriPathSegmentStructA *head(xs);
  std::string accum;

  while (head) {
    accum += delim + toString(head->text);
    head = head->next;
  }

  return accum;
}
}  // namespace

namespace Xi {
namespace Network {

struct Uri::_Impl {
  std::string raw{};
  std::string scheme{};
  std::string host{};
  Port port{};
  std::string path{};
  std::string query{};
  std::string fragment{};
};

Result<Uri> Uri::fromString(const std::string &str) {
  XI_ERROR_TRY
  Uri reval{};
  UriUriA native{};
  const char *errorPosition = nullptr;
  if (const auto ec = uriParseSingleUriA(&native, str.c_str(), &errorPosition); ec != URI_SUCCESS) {
    exceptional<InvalidArgumentError>("'{}' is not a valid uri.", str);
  } else {
    reval.m_impl->raw = str;
    reval.m_impl->scheme = toString(native.scheme);
    reval.m_impl->host = toString(native.hostText);
    reval.m_impl->path = toString(native.pathHead, "/");
    reval.m_impl->query = toString(native.query);
    reval.m_impl->fragment = toString(native.fragment);

    {
      std::string port = toString(native.portText);
      if (port.empty()) {
        reval.m_impl->port = Port::Any;
      } else {
        reval.m_impl->port = Port::fromString(port).valueOrThrow();
      }
    }

    return success(std::move(reval));
  }
  XI_ERROR_CATCH
}

Uri::Uri() : m_impl{new _Impl} {
  /* */
}

Uri::Uri(const Uri &other) : m_impl{new _Impl{*other.m_impl}} {
  /* */
}

Uri &Uri::operator=(const Uri &other) {
  if (this != std::addressof(other)) {
    *m_impl = *other.m_impl;
  }
  return *this;
}

Uri::Uri(Uri &&other) : m_impl{std::move(other.m_impl)} {
  /* */
}

Uri &Uri::operator=(Uri &&other) {
  if (this != std::addressof(other)) {
    m_impl = std::move(other.m_impl);
  }
  return *this;
}

Uri::~Uri() {
  /* */
}

const std::string &Uri::scheme() const {
  return m_impl->scheme;
}

const std::string &Uri::host() const {
  return m_impl->host;
}

Port Uri::port() const {
  return m_impl->port;
}

const std::string &Uri::path() const {
  return m_impl->path;
}

const std::string &Uri::query() const {
  return m_impl->query;
}

const std::string &Uri::fragment() const {
  return m_impl->fragment;
}

}  // namespace Network
}  // namespace Xi
