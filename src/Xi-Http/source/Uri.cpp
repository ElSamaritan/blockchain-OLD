// Copyright (C) 2015 Ben Lewis <benjf5+github@gmail.com>
// Licensed under the MIT license.
// This source has been altered by the Galaxia Project Developers

#include "Xi/Http/Uri.h"

#include "Xi/Algorithm/String.h"

Xi::Http::Uri::Uri(const char *uri_text, Xi::Http::Uri::SchemeCategory category,
                   Xi::Http::Uri::QueryArgumentSeparator separator)
    : m_category(category), m_port(0), m_path_is_rooted(false), m_separator(separator) {
  setUp(std::string(uri_text), category);
}

Xi::Http::Uri::Uri(const std::string &uri_text, Xi::Http::Uri::SchemeCategory category,
                   Xi::Http::Uri::QueryArgumentSeparator separator)
    : m_category(category), m_port(0), m_path_is_rooted(false), m_separator(separator) {
  setUp(uri_text, category);
}

Xi::Http::Uri::Uri(const std::map<Xi::Http::Uri::Component, std::string> &components,
                   Xi::Http::Uri::SchemeCategory category, bool rooted_path,
                   Xi::Http::Uri::QueryArgumentSeparator separator)
    : m_category(category), m_path_is_rooted(rooted_path), m_separator(separator) {
  if (components.count(Component::Scheme)) {
    if (components.at(Component::Scheme).length() == 0) {
      throw std::invalid_argument("Scheme cannot be empty.");
    }
    m_scheme = components.at(Component::Scheme);
  } else {
    throw std::invalid_argument("A URI must have a scheme.");
  }

  if (category == SchemeCategory::Hierarchical) {
    if (components.count(Component::Content)) {
      throw std::invalid_argument("The content component is only for use in non-hierarchical URIs.");
    }

    bool has_username = components.count(Component::Username);
    bool has_password = components.count(Component::Password);
    if (has_username && has_password) {
      m_username = components.at(Component::Username);
      m_password = components.at(Component::Password);
    } else if ((has_username && !has_password) || (!has_username && has_password)) {
      throw std::invalid_argument("If a username or password is supplied, both must be provided.");
    }

    if (components.count(Component::Host)) {
      m_host = components.at(Component::Host);
    }

    if (components.count(Component::Port)) {
      m_port = lexical_cast<uint16_t>(components.at(Component::Port));
    }

    if (components.count(Component::Path)) {
      m_path = components.at(Component::Path);
    } else {
      throw std::invalid_argument("A path is required on a hierarchical URI, even an empty path.");
    }
  } else {
    if (components.count(Component::Username) || components.count(Component::Password) ||
        components.count(Component::Host) || components.count(Component::Port) || components.count(Component::Path)) {
      throw std::invalid_argument("None of the hierarchical components are allowed in a non-hierarchical URI.");
    }

    if (components.count(Component::Content)) {
      m_content = components.at(Component::Content);
    } else {
      throw std::invalid_argument("Content is a required component for a non-hierarchical URI, even an empty string.");
    }
  }

  if (components.count(Component::Query)) {
    m_query = components.at(Component::Query);
  }

  if (components.count(Component::Fragment)) {
    m_fragment = components.at(Component::Fragment);
  }
}

Xi::Http::Uri::Uri(const Uri &other, const std::map<Xi::Http::Uri::Component, std::string> &replacements)
    : m_category(other.m_category), m_path_is_rooted(other.m_path_is_rooted), m_separator(other.m_separator) {
  m_scheme = (replacements.count(Component::Scheme)) ? replacements.at(Component::Scheme) : other.m_scheme;

  if (m_category == SchemeCategory::Hierarchical) {
    m_username = (replacements.count(Component::Username)) ? replacements.at(Component::Username) : other.m_username;

    m_password = (replacements.count(Component::Password)) ? replacements.at(Component::Password) : other.m_password;

    m_host = (replacements.count(Component::Host)) ? replacements.at(Component::Host) : other.m_host;

    m_port =
        (replacements.count(Component::Port)) ? lexical_cast<uint16_t>(replacements.at(Component::Port)) : other.m_port;

    m_path = (replacements.count(Component::Path)) ? replacements.at(Component::Path) : other.m_path;
  } else {
    m_content = (replacements.count(Component::Content)) ? replacements.at(Component::Content) : other.m_content;
  }

  m_query = (replacements.count(Component::Query)) ? replacements.at(Component::Query) : other.m_query;

  m_fragment = (replacements.count(Component::Fragment)) ? replacements.at(Component::Fragment) : other.m_fragment;
}

const std::string &Xi::Http::Uri::scheme() const {
  return m_scheme;
}

Xi::Http::Uri::SchemeCategory Xi::Http::Uri::schemeCategory() const {
  return m_category;
}

const std::string &Xi::Http::Uri::content() const {
  if (m_category != SchemeCategory::NonHierarchical) {
    throw std::domain_error("The content component is only valid for non-hierarchical URIs.");
  }
  return m_content;
}

const std::string &Xi::Http::Uri::username() const {
  if (m_category != SchemeCategory::Hierarchical) {
    throw std::domain_error("The username component is only valid for hierarchical URIs.");
  }
  return m_username;
}

const std::string &Xi::Http::Uri::password() const {
  if (m_category != SchemeCategory::Hierarchical) {
    throw std::domain_error("The password component is only valid for hierarchical URIs.");
  }
  return m_password;
}

const std::string &Xi::Http::Uri::host() const {
  if (m_category != SchemeCategory::Hierarchical) {
    throw std::domain_error("The host component is only valid for hierarchical URIs.");
  }
  return m_host;
}

uint16_t Xi::Http::Uri::port() const {
  if (m_category != SchemeCategory::Hierarchical) {
    throw std::domain_error("The port component is only valid for hierarchical URIs.");
  }
  return m_port;
}

const std::string &Xi::Http::Uri::path() const {
  if (m_category != SchemeCategory::Hierarchical) {
    throw std::domain_error("The path component is only valid for hierarchical URIs.");
  }
  return m_path;
}

const std::string &Xi::Http::Uri::query() const {
  return m_query;
}

const std::map<std::string, std::string> &Xi::Http::Uri::queryDictionary() const {
  return m_queryDict;
}

const std::string &Xi::Http::Uri::fragment() const {
  return m_fragment;
}

std::string Xi::Http::Uri::toString() const {
  std::string full_uri;
  full_uri.append(m_scheme);
  full_uri.append(":");

  if (m_content.length() > m_path.length()) {
    full_uri.append("//");
    if (!(m_username.empty() || m_password.empty())) {
      full_uri.append(m_username);
      full_uri.append(":");
      full_uri.append(m_password);
      full_uri.append("@");
    }

    full_uri.append(m_host);

    if (m_port != 0) {
      full_uri.append(":");
      full_uri.append(std::to_string(m_port));
    }
  }

  if (m_path_is_rooted) {
    full_uri.append("/");
  }
  full_uri.append(m_path);

  if (!m_query.empty()) {
    full_uri.append("?");
    full_uri.append(m_query);
  }

  if (!m_fragment.empty()) {
    full_uri.append("#");
    full_uri.append(m_fragment);
  }

  return full_uri;
}

std::string Xi::Http::Uri::relativePart() const {
  auto relativePart = std::string{"/"} + path();
  if (!query().empty())
    relativePart += std::string{"?"} + query();
  if (!fragment().empty())
    relativePart += std::string{"#"} + fragment();
  return relativePart;
}

void Xi::Http::Uri::setUp(const std::string &uri_text, Xi::Http::Uri::SchemeCategory category) {
  XI_UNUSED(category);
  size_t const uri_length = uri_text.length();

  if (uri_length == 0) {
    throw std::invalid_argument("URIs cannot be of zero length.");
  }

  std::string::const_iterator cursor = parseScheme(uri_text, uri_text.begin());
  // After calling parse_scheme, *cursor == ':'; none of the following parsers
  // expect a separator character, so we advance the cursor upon calling them.
  cursor = parseContent(uri_text, (cursor + 1));

  if ((cursor != uri_text.end()) && (*cursor == '?')) {
    cursor = parseQuery(uri_text, (cursor + 1));
  }

  if ((cursor != uri_text.end()) && (*cursor == '#')) {
    cursor = parseFragment(uri_text, (cursor + 1));
  }

  initQueryDictionary();  // If the query string is empty, this will be empty too.
}

std::string::const_iterator Xi::Http::Uri::parseScheme(const std::string &uri_text,
                                                       std::string::const_iterator scheme_start) {
  std::string::const_iterator scheme_end = scheme_start;
  while ((scheme_end != uri_text.end()) && (*scheme_end != ':')) {
    if (!(std::isalnum(*scheme_end) || (*scheme_end == '-') || (*scheme_end == '+') || (*scheme_end == '.'))) {
      throw std::invalid_argument("Invalid character found in the scheme component. Supplied URI was: \"" + uri_text +
                                  "\".");
    }
    ++scheme_end;
  }

  if (scheme_end == uri_text.end()) {
    throw std::invalid_argument("End of URI found while parsing the scheme. Supplied URI was: \"" + uri_text + "\".");
  }

  if (scheme_start == scheme_end) {
    throw std::invalid_argument("Scheme component cannot be zero-length. Supplied URI was: \"" + uri_text + "\".");
  }

  m_scheme = std::string(scheme_start, scheme_end);
  return scheme_end;
}

std::string::const_iterator Xi::Http::Uri::parseContent(const std::string &uri_text,
                                                        std::string::const_iterator content_start) {
  std::string::const_iterator content_end = content_start;
  while ((content_end != uri_text.end()) && (*content_end != '?') && (*content_end != '#')) {
    ++content_end;
  }

  m_content = std::string(content_start, content_end);

  if ((m_category == SchemeCategory::Hierarchical) && (m_content.length() > 0)) {
    // If it's a hierarchical URI, the content should be parsed for the hierarchical components.
    std::string::const_iterator path_start = m_content.begin();
    std::string::const_iterator path_end = m_content.end();
    if (!m_content.compare(0, 2, "//")) {
      // In this case an authority component is present.
      std::string::const_iterator authority_cursor = (m_content.begin() + 2);
      if (m_content.find_first_of('@') != std::string::npos) {
        std::string::const_iterator userpass_divider = parseUsername(uri_text, m_content, authority_cursor);
        authority_cursor = parsePassword(uri_text, m_content, (userpass_divider + 1));
        // After this call, *authority_cursor == '@', so we skip over it.
        ++authority_cursor;
      }

      authority_cursor = parseHost(uri_text, m_content, authority_cursor);

      if (authority_cursor != m_content.end() && *authority_cursor == ':') {
        authority_cursor = parsePort(uri_text, m_content, (authority_cursor + 1));
      }

      if ((authority_cursor != m_content.end() && *authority_cursor == '/') || !m_host.empty()) {
        // Then the path is rooted, and we should note this.
        m_path_is_rooted = true;
        if (authority_cursor != m_content.end())
          path_start = authority_cursor + 1;
        else
          authority_cursor = m_content.end();
      }
    } else if (!m_content.compare(0, 1, "/")) {
      m_path_is_rooted = true;
      ++path_start;
    }

    // We can now build the path based on what remains in the content string,
    // since that's all that exists after the host and optional port component.
    m_path = std::string(path_start, path_end);
  }
  return content_end;
}

std::string::const_iterator Xi::Http::Uri::parseUsername(const std::string &uri_text, const std::string &content,
                                                         std::string::const_iterator username_start) {
  XI_UNUSED(content);
  std::string::const_iterator username_end = username_start;
  // Since this is only reachable when '@' was in the content string, we can
  // ignore the end-of-string case.
  while (*username_end != ':') {
    if (*username_end == '@') {
      throw std::invalid_argument("Username must be followed by a password. Supplied URI was: \"" + uri_text + "\".");
    }
    ++username_end;
  }
  m_username = std::string(username_start, username_end);
  return username_end;
}

std::string::const_iterator Xi::Http::Uri::parsePassword(const std::string &uri_text, const std::string &content,
                                                         std::string::const_iterator password_start) {
  XI_UNUSED(uri_text, content);
  std::string::const_iterator password_end = password_start;
  while (*password_end != '@') {
    ++password_end;
  }

  m_password = std::string(password_start, password_end);
  return password_end;
}

std::string::const_iterator Xi::Http::Uri::parseHost(const std::string &uri_text, const std::string &content,
                                                     std::string::const_iterator host_start) {
  std::string::const_iterator host_end = host_start;
  // So, the host can contain a few things. It can be a domain, it can be an
  // IPv4 address, it can be an IPv6 address, or an IPvFuture literal. In the
  // case of those last two, it's of the form [...] where what's between the
  // brackets is a matter of which IPv?? version it is.
  while (host_end != content.end()) {
    if (*host_end == '[') {
      // We're parsing an IPv6 or IPvFuture address, so we should handle that
      // instead of the normal procedure.
      while ((host_end != content.end()) && (*host_end != ']')) {
        ++host_end;
      }

      if (host_end == content.end()) {
        throw std::invalid_argument(
            "End of content component encountered "
            "while parsing the host component. "
            "Supplied URI was: \"" +
            uri_text + "\".");
      }

      ++host_end;
      break;
      // We can stop looping, we found the end of the IP literal, which is the
      // whole of the host component when one's in use.
    } else if ((*host_end == ':') || (*host_end == '/')) {
      break;
    } else {
      ++host_end;
    }
  }

  m_host = std::string(host_start, host_end);
  return host_end;
}

std::string::const_iterator Xi::Http::Uri::parsePort(const std::string &uri_text, const std::string &content,
                                                     std::string::const_iterator port_start) {
  std::string::const_iterator port_end = port_start;
  while ((port_end != content.end()) && (*port_end != '/')) {
    if (!std::isdigit(*port_end)) {
      throw std::invalid_argument(
          "Invalid character while parsing the port. "
          "Supplied URI was: \"" +
          uri_text + "\".");
    }

    ++port_end;
  }

  m_port = lexical_cast<uint16_t>(std::string(port_start, port_end));
  return port_end;
}

std::string::const_iterator Xi::Http::Uri::parseQuery(const std::string &uri_text,
                                                      std::string::const_iterator query_start) {
  std::string::const_iterator query_end = query_start;
  while ((query_end != uri_text.end()) && (*query_end != '#')) {
    // Queries can contain almost any character except hash, which is reserved
    // for the start of the fragment.
    ++query_end;
  }
  m_query = std::string(query_start, query_end);
  return query_end;
}

std::string::const_iterator Xi::Http::Uri::parseFragment(const std::string &uri_text,
                                                         std::string::const_iterator fragment_start) {
  m_fragment = std::string(fragment_start, uri_text.end());
  return uri_text.end();
}

void Xi::Http::Uri::initQueryDictionary() {
  if (!m_query.empty()) {
    // Loop over the query string looking for '&'s, then check each one for
    // an '=' to find keys and values; if there's not an '=' then the key
    // will have an empty value in the map.
    char separator = (m_separator == QueryArgumentSeparator::Ampersand) ? '&' : ';';
    size_t carat = 0;
    size_t stanza_end = m_query.find_first_of(separator);
    do {
      std::string stanza =
          m_query.substr(carat, ((stanza_end != std::string::npos) ? (stanza_end - carat) : std::string::npos));
      size_t key_value_divider = stanza.find_first_of('=');
      std::string key = stanza.substr(0, key_value_divider);
      std::string value;
      if (key_value_divider != std::string::npos) {
        value = stanza.substr((key_value_divider + 1));
      }

      if (m_queryDict.count(key) != 0) {
        throw std::invalid_argument("Bad key in the query string!");
      }

      m_queryDict.emplace(key, value);
      carat = ((stanza_end != std::string::npos) ? (stanza_end + 1) : std::string::npos);
      stanza_end = m_query.find_first_of(separator, carat);
    } while ((stanza_end != std::string::npos) || (carat != std::string::npos));
  }
}
