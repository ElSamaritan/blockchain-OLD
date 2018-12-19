#pragma once

#include <string>

#include <Xi/Utils/String.h>

namespace Xi {
namespace Http {
/*!
 * \brief The AuthenticationType enum represents a type of authentication required/provided
 */
enum struct AuthenticationType { Basic, Unsupported };
}  // namespace Http
std::string to_string(Http::AuthenticationType status);

template <>
Xi::Http::AuthenticationType lexical_cast<Xi::Http::AuthenticationType>(const std::string& value);
}  // namespace Xi
