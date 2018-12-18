#include "Xi/Http/BasicCredentials.h"

#include <string>
#include <algorithm>
#include <stdexcept>

bool Xi::Http::BasicCredentials::validateUsername(const std::string &username) {
  return !username.empty() && std::find(username.begin(), username.end(), ':') == username.end();
}

Xi::Http::BasicCredentials::BasicCredentials(const std::string &_username, const std::string &_password)
    : m_username{_username}, m_password{_password} {
  if (!validateUsername(_username)) throw std::invalid_argument{"provided username is invalid."};
}

void Xi::Http::BasicCredentials::setUsername(const std::string &_username) {
  if (!validateUsername(_username)) throw std::invalid_argument{"provided username is invalid."};
  m_username = _username;
}

const std::string &Xi::Http::BasicCredentials::username() const { return m_username; }

void Xi::Http::BasicCredentials::setPassword(const std::string &_password) { m_password = _password; }

const std::string &Xi::Http::BasicCredentials::password() const { return m_password; }

namespace Xi {
std::string to_string(const Xi::Http::BasicCredentials &cred) { return cred.username() + ":" + cred.password(); }

template <>
Xi::Http::BasicCredentials lexical_cast<Xi::Http::BasicCredentials>(const std::string &value) {
  const auto splitPos = std::find(value.begin(), value.end(), ':');
  if (splitPos == value.end()) throw std::invalid_argument{"provided credentials are not correctly encoded"};
  const std::string username{value.begin(), splitPos};
  const std::string password{splitPos + 1, value.end()};
  return Http::BasicCredentials{username, password};
}
}  // namespace Xi
