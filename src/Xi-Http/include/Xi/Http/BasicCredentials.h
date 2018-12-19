#pragma once

#include <string>
#include <stdexcept>

#include <Xi/Global.h>
#include <Xi/Utils/String.h>

namespace Xi {
namespace Http {
/*!
 * \brief The BasicCredentials class wraps a basic username and password authentication
 */
class BasicCredentials final {
 public:
  static bool validateUsername(const std::string& username);

 public:
  XI_DEFAULT_COPY(BasicCredentials);
  XI_DEFAULT_MOVE(BasicCredentials);

  /*!
   * \brief BasicCredentials construct authentication credentials required for basic http authorization
   * \param username The username to be used cannot be empty or contain ':'
   * \param password The password to be used, can be empty
   * \exception std::invalid_argument username is empty or contains ':'
   */
  explicit BasicCredentials(const std::string& username, const std::string& password = "");
  ~BasicCredentials() = default;

  /*!
   * \brief setUsername sets the username required.
   * \exception std::invalid_argument username is empty or contains ':'
   */
  void setUsername(const std::string& username);

  const std::string& username() const;

  void setPassword(const std::string& password);

  const std::string& password() const;

 private:
  std::string m_username;
  std::string m_password;
};
}  // namespace Http
std::string to_string(const Http::BasicCredentials& status);
template <>
Xi::Http::BasicCredentials lexical_cast<Xi::Http::BasicCredentials>(const std::string& value);
}  // namespace Xi
