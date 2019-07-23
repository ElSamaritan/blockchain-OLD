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

#include "Xi/Http/HeaderContainer.h"

#include <sstream>
#include <stdexcept>
#include <iterator>

#include <boost/algorithm/string.hpp>
#include <boost/beast.hpp>

#include <Xi/Encoding/Base64.h>

static_assert(
    static_cast<boost::beast::http::field>(Xi::Http::HeaderContainer::WWWAuthenticate) ==
        boost::beast::http::field::www_authenticate,
    "The casted value must correspond to the enum used by the beast library because they are internally casted.");
static_assert(
    static_cast<boost::beast::http::field>(Xi::Http::HeaderContainer::Authorization) ==
        boost::beast::http::field::authorization,
    "The casted value must correspond to the enum used by the beast library because they are internally casted.");
static_assert(
    static_cast<boost::beast::http::field>(Xi::Http::HeaderContainer::Connection) ==
        boost::beast::http::field::connection,
    "The casted value must correspond to the enum used by the beast library because they are internally casted.");
static_assert(
    static_cast<boost::beast::http::field>(Xi::Http::HeaderContainer::KeepAlive) ==
        boost::beast::http::field::keep_alive,
    "The casted value must correspond to the enum used by the beast library because they are internally casted.");
static_assert(
    static_cast<boost::beast::http::field>(Xi::Http::HeaderContainer::Accept) == boost::beast::http::field::accept,
    "The casted value must correspond to the enum used by the beast library because they are internally casted.");
static_assert(
    static_cast<boost::beast::http::field>(Xi::Http::HeaderContainer::AcceptCharset) ==
        boost::beast::http::field::accept_charset,
    "The casted value must correspond to the enum used by the beast library because they are internally casted.");
static_assert(
    static_cast<boost::beast::http::field>(Xi::Http::HeaderContainer::AcceptEncoding) ==
        boost::beast::http::field::accept_encoding,
    "The casted value must correspond to the enum used by the beast library because they are internally casted.");
static_assert(
    static_cast<boost::beast::http::field>(Xi::Http::HeaderContainer::ContentType) ==
        boost::beast::http::field::content_type,
    "The casted value must correspond to the enum used by the beast library because they are internally casted.");
static_assert(
    static_cast<boost::beast::http::field>(Xi::Http::HeaderContainer::ContentEncoding) ==
        boost::beast::http::field::content_encoding,
    "The casted value must correspond to the enum used by the beast library because they are internally casted.");
static_assert(
    static_cast<boost::beast::http::field>(Xi::Http::HeaderContainer::Location) == boost::beast::http::field::location,
    "The casted value must correspond to the enum used by the beast library because they are internally casted.");
static_assert(
    static_cast<boost::beast::http::field>(Xi::Http::HeaderContainer::Allow) == boost::beast::http::field::allow,
    "The casted value must correspond to the enum used by the beast library because they are internally casted.");
static_assert(
    static_cast<boost::beast::http::field>(Xi::Http::HeaderContainer::Server) == boost::beast::http::field::server,
    "The casted value must correspond to the enum used by the beast library because they are internally casted.");
static_assert(
    static_cast<boost::beast::http::field>(Xi::Http::HeaderContainer::AccessControlAllowOrigin) ==
        boost::beast::http::field::access_control_allow_origin,
    "The casted value must correspond to the enum used by the beast library because they are internally casted.");
static_assert(
    static_cast<boost::beast::http::field>(Xi::Http::HeaderContainer::AccessControlAllowMethods) ==
        boost::beast::http::field::access_control_allow_methods,
    "The casted value must correspond to the enum used by the beast library because they are internally casted.");

void Xi::Http::HeaderContainer::setRequiredAuthenticationScheme(Xi::Http::AuthenticationType authType) {
  set(WWWAuthenticate, to_string(authType));
}

boost::optional<Xi::Http::AuthenticationType> Xi::Http::HeaderContainer::requiredAuthenticationScheme() const {
  const auto search = get(WWWAuthenticate);
  if (search)
    return lexical_cast<Xi::Http::AuthenticationType>(*search);
  else
    return boost::optional<Xi::Http::AuthenticationType>{};
}

void Xi::Http::HeaderContainer::setBasicAuthorization(const std::string &username, const std::string &password) {
  setBasicAuthorization(BasicCredentials{username, password});
}

void Xi::Http::HeaderContainer::setBasicAuthorization(const Xi::Http::BasicCredentials &credentials) {
  set(Authorization, to_string(AuthenticationType::Basic) + " " + Base64::encode(to_string(credentials)));
}

void Xi::Http::HeaderContainer::setBearerAuthorization(const std::string &token) {
  BearerCredentials cred{token};
  setBearerAuthorization(cred);
}

void Xi::Http::HeaderContainer::setBearerAuthorization(const Xi::Http::BearerCredentials &bearer) {
  set(Authorization, to_string(AuthenticationType::Bearer) + " " + bearer.token());
}

void Xi::Http::HeaderContainer::setAllow(std::initializer_list<Method> method) {
  if (method.size() == 0)
    throw std::invalid_argument{"you need to support at least one method."};
  std::stringstream builder{};
  builder << to_string(*method.begin());
  for (auto it = std::next(method.begin()); it != method.end(); ++it)
    builder << ", " << to_string(*it);
  set(Allow, builder.str());
}

boost::optional<Xi::Http::BasicCredentials> Xi::Http::HeaderContainer::basicAuthorization() const {
  const auto search = get(Authorization);
  if (search) {
    const std::string authPrefix = to_string(AuthenticationType::Basic) + " ";
    if (!Xi::starts_with(*search, authPrefix)) {
      return boost::optional<BasicCredentials>{};
    }
    const std::string encodedAuth = search->substr(authPrefix.size());
    const std::string decodedAuth = Base64::decode(encodedAuth);
    return lexical_cast<BasicCredentials>(decodedAuth);
  } else
    return boost::optional<BasicCredentials>{};
}

std::optional<Xi::Http::BearerCredentials> Xi::Http::HeaderContainer::bearerAuthorization() const {
  const auto search = get(Authorization);
  if (search) {
    const std::string authPrefix = to_string(AuthenticationType::Bearer) + " ";
    if (!Xi::starts_with(*search, authPrefix)) {
      return std::nullopt;
    }
    const std::string auth = search->substr(authPrefix.size());
    return lexical_cast<BearerCredentials>(auth);
  } else
    return std::nullopt;
}

void Xi::Http::HeaderContainer::setContentType(Xi::Http::ContentType _contentType) {
  set(ContentType, to_string(_contentType));
}

void Xi::Http::HeaderContainer::setAcceptedContentEncodings(
    std::initializer_list<Xi::Http::ContentEncoding> encodings) {
  if (encodings.size() == 0)
    throw std::invalid_argument{"you need to support at least one encoding."};
  std::stringstream builder{};
  builder << to_string(*encodings.begin());
  for (auto it = std::next(encodings.begin()); it != encodings.end(); ++it)
    builder << ", " << to_string(*it);
  set(AcceptEncoding, builder.str());
}

boost::optional<std::vector<Xi::Http::ContentEncoding> > Xi::Http::HeaderContainer::acceptedContentEncodings() const {
  const auto search = get(AcceptEncoding);
  if (search) {
    std::string encodings = *search;
    std::string weightings = "";
    auto findWeightingDelimiter = encodings.find(";");
    if (findWeightingDelimiter != std::string::npos) {
      weightings = encodings.substr(findWeightingDelimiter + 1);
      encodings = encodings.substr(0, findWeightingDelimiter);
    }
    std::vector<std::string> supportedEncodings;
    boost::split(supportedEncodings, *search, boost::is_any_of(","));
    for (auto &encoding : supportedEncodings)
      boost::trim(encoding);
    std::vector<Xi::Http::ContentEncoding> reval;
    std::transform(supportedEncodings.begin(), supportedEncodings.end(), std::back_inserter(reval),
                   lexical_cast<Xi::Http::ContentEncoding>);
    return std::move(reval);
  } else
    return boost::optional<std::vector<Xi::Http::ContentEncoding> >{};
}

bool Xi::Http::HeaderContainer::acceptsContentEncoding(Xi::Http::ContentEncoding encoding) const {
  const auto acceptedEncodings = acceptedContentEncodings();
  if (acceptedEncodings) {
    return std::find(acceptedEncodings->begin(), acceptedEncodings->end(), encoding) != acceptedEncodings->end();
  } else {
    return false;
  }
}

void Xi::Http::HeaderContainer::setContentEncoding(Xi::Http::ContentEncoding encoding) {
  set(ContentEncoding, to_string(encoding));
}

boost::optional<Xi::Http::ContentEncoding> Xi::Http::HeaderContainer::contentEncoding() const {
  const auto search = get(ContentEncoding);
  if (search)
    return lexical_cast<Xi::Http::ContentEncoding>(*search);
  else
    return boost::optional<Xi::Http::ContentEncoding>{};
}

boost::optional<Xi::Http::ContentType> Xi::Http::HeaderContainer::contentType() const {
  const auto search = get(ContentType);
  if (search) {
    const auto raw = *search;
    auto seperator = std::find(raw.begin(), raw.end(), ';');
    return lexical_cast<Xi::Http::ContentType>(std::string{raw.begin(), seperator});
  } else
    return boost::optional<Xi::Http::ContentType>{};
}

boost::optional<std::string> Xi::Http::HeaderContainer::location() const {
  auto search = m_rawHeaders.find(Location);
  if (search == m_rawHeaders.end())
    return boost::optional<std::string>{};
  else
    return boost::optional<std::string>{search->second};
}

Xi::Http::HeaderContainer::const_iterator Xi::Http::HeaderContainer::begin() const {
  return m_rawHeaders.begin();
}

Xi::Http::HeaderContainer::const_iterator Xi::Http::HeaderContainer::end() const {
  return m_rawHeaders.end();
}

boost::optional<std::string> Xi::Http::HeaderContainer::get(Xi::Http::HeaderContainer::Header header) const {
  auto search = m_rawHeaders.find(header);
  if (search == m_rawHeaders.end())
    return boost::optional<std::string>{};
  else
    return boost::optional<std::string>{search->second};
}

void Xi::Http::HeaderContainer::setAccessControlRequestMethods(std::initializer_list<Xi::Http::Method> methods) {
  if (methods.size() == 0)
    throw std::invalid_argument{"you need to support at least one method."};
  std::stringstream builder{};
  builder << to_string(*methods.begin());
  for (auto it = std::next(methods.begin()); it != methods.end(); ++it)
    builder << ", " << to_string(*it);
  set(AccessControlAllowMethods, builder.str());
}

void Xi::Http::HeaderContainer::set(Xi::Http::HeaderContainer::Header header, const std::string &raw) {
  m_rawHeaders[header] = raw;
}

bool Xi::Http::HeaderContainer::contains(Xi::Http::HeaderContainer::Header header) const {
  return m_rawHeaders.find(header) != m_rawHeaders.end();
}
