// Copyright (C) 2015 Ben Lewis <benjf5+github@gmail.com>
// Licensed under the MIT license.
// This source has been altered by the Galaxia Project Developers

#pragma once

#include <cctype>
#include <map>
#include <string>
#include <stdexcept>
#include <utility>
#include <cinttypes>

#include <Xi/Global.h>

namespace Xi {
namespace Http {
class Uri {
  /* URIs are broadly divided into two categories: hierarchical and
   * non-hierarchical. Both hierarchical URIs and non-hierarchical URIs have a
   * few elements in common; all URIs have a scheme of one or more alphanumeric
   * characters followed by a colon, and they all may optionally have a query
   * component preceded by a question mark, and a fragment component preceded by
   * an octothorpe (hash mark: '#'). The query consists of stanzas separated by
   * either ampersands ('&') or semicolons (';') (but only one or the other),
   * and each stanza consists of a key and an optional value; if the value
   * exists, the key and value must be divided by an equals sign.
   *
   * The following is an example from Wikipedia of a hierarchical URI:
   * scheme:[//[user:password@]domain[:port]][/]path[?query][#fragment]
   */

 public:
  enum struct SchemeCategory { Hierarchical, NonHierarchical };

  enum struct Component { Scheme, Content, Username, Password, Host, Port, Path, Query, Fragment };

  enum struct QueryArgumentSeparator { Ampersand, Semicolon };

  Uri(char const *uri_text, SchemeCategory category = SchemeCategory::Hierarchical,
      QueryArgumentSeparator separator = QueryArgumentSeparator::Ampersand);

  Uri(std::string const &uri_text, SchemeCategory category = SchemeCategory::Hierarchical,
      QueryArgumentSeparator separator = QueryArgumentSeparator::Ampersand);

  Uri(std::map<Component, std::string> const &components, SchemeCategory category, bool rooted_path,
      QueryArgumentSeparator separator = QueryArgumentSeparator::Ampersand);

  Uri(Uri const &other, std::map<Component, std::string> const &replacements);

  XI_DEFAULT_COPY(Uri);
  XI_DEFAULT_MOVE(Uri);

  ~Uri() = default;

  std::string const &scheme() const;

  SchemeCategory schemeCategory() const;

  std::string const &content() const;

  std::string const &username() const;

  std::string const &password() const;

  std::string const &host() const;

  uint16_t port() const;

  std::string const &path() const;

  std::string const &query() const;

  std::map<std::string, std::string> const &queryDictionary() const;

  std::string const &fragment() const;

  std::string toString() const;

 private:
  void setUp(std::string const &uri_text, SchemeCategory category);

  std::string::const_iterator parseScheme(std::string const &uri_text, std::string::const_iterator scheme_start);

  std::string::const_iterator parseContent(std::string const &uri_text, std::string::const_iterator content_start);

  std::string::const_iterator parseUsername(std::string const &uri_text, std::string const &content,
                                            std::string::const_iterator username_start);

  std::string::const_iterator parsePassword(std::string const &uri_text, std::string const &content,
                                            std::string::const_iterator password_start);

  std::string::const_iterator parseHost(std::string const &uri_text, std::string const &content,
                                        std::string::const_iterator host_start);

  std::string::const_iterator parsePort(std::string const &uri_text, std::string const &content,
                                        std::string::const_iterator port_start);

  std::string::const_iterator parseQuery(std::string const &uri_text, std::string::const_iterator query_start);

  std::string::const_iterator parseFragment(std::string const &uri_text, std::string::const_iterator fragment_start);

  void initQueryDictionary();

  std::string m_scheme;
  std::string m_content;
  std::string m_username;
  std::string m_password;
  std::string m_host;
  std::string m_path;
  std::string m_query;
  std::string m_fragment;

  std::map<std::string, std::string> m_queryDict;

  SchemeCategory m_category;
  uint16_t m_port;
  bool m_path_is_rooted;
  QueryArgumentSeparator m_separator;
};
}  // namespace Http
}  // namespace Xi
