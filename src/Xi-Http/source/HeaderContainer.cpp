#include "Xi/Http/HeaderContainer.h"

#include <sstream>
#include <stdexcept>
#include <iterator>

#include <boost/algorithm/string.hpp>
#include <boost/beast.hpp>

#include <Xi/Utils/Base64.h>

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

boost::optional<Xi::Http::BasicCredentials> Xi::Http::HeaderContainer::basicAuthorization() const {
  const auto search = get(Authorization);
  if (search) {
    const std::string authPrefix = to_string(AuthenticationType::Basic) + " ";
    if (!Xi::starts_with(*search, authPrefix))
      throw std::runtime_error{"invalid basic authentication scheme provided."};
    const std::string encodedAuth = search->substr(authPrefix.size());
    const std::string decodedAuth = Base64::decode(encodedAuth);
    return lexical_cast<BasicCredentials>(decodedAuth);
  } else
    return boost::optional<BasicCredentials>{};
}

void Xi::Http::HeaderContainer::setContentType(Xi::Http::ContentType _contentType) {
  set(ContentType, to_string(_contentType));
}

void Xi::Http::HeaderContainer::setAcceptedContentEncodings(
    std::initializer_list<Xi::Http::ContentEncoding> encodings) {
  if (encodings.size() == 0) throw std::invalid_argument{"you need to support at least one encoding."};
  std::stringstream builder{};
  builder << to_string(*encodings.begin());
  for (auto it = std::next(encodings.begin()); it != encodings.end(); ++it) builder << ", " << to_string(*it);
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
    for (auto &encoding : supportedEncodings) boost::trim(encoding);
    std::vector<Xi::Http::ContentEncoding> reval;
    std::transform(supportedEncodings.begin(), supportedEncodings.end(), std::back_inserter(reval),
                   lexical_cast<Xi::Http::ContentEncoding>);
    return reval;
  } else
    return boost::optional<std::vector<Xi::Http::ContentEncoding> >{};
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
  if (search)
    return lexical_cast<Xi::Http::ContentType>(*search);
  else
    return boost::optional<Xi::Http::ContentType>{};
}

boost::optional<std::string> Xi::Http::HeaderContainer::location() const {
  auto search = m_rawHeaders.find(Location);
  if (search == m_rawHeaders.end())
    return boost::optional<std::string>{};
  else
    return boost::optional<std::string>{search->second};
}

Xi::Http::HeaderContainer::const_iterator Xi::Http::HeaderContainer::begin() const { return m_rawHeaders.begin(); }

Xi::Http::HeaderContainer::const_iterator Xi::Http::HeaderContainer::end() const { return m_rawHeaders.end(); }

boost::optional<std::string> Xi::Http::HeaderContainer::get(Xi::Http::HeaderContainer::Header header) const {
  auto search = m_rawHeaders.find(header);
  if (search == m_rawHeaders.end())
    return boost::optional<std::string>{};
  else
    return boost::optional<std::string>{search->second};
}

void Xi::Http::HeaderContainer::set(Xi::Http::HeaderContainer::Header header, const std::string &raw) {
  m_rawHeaders[header] = raw;
}

bool Xi::Http::HeaderContainer::contains(Xi::Http::HeaderContainer::Header header) const {
  return m_rawHeaders.find(header) != m_rawHeaders.end();
}
