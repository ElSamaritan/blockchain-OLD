#include "Xi/Http/AuthenticationType.h"

#include <stdexcept>

std::string Xi::to_string(Xi::Http::AuthenticationType status) {
  switch (status) {
    case (Xi::Http::AuthenticationType::Basic):
      return "Basic";
    case (Xi::Http::AuthenticationType::Unsupported):
      return "Unsupported";
    default:
      throw std::runtime_error{"invalid authentication type provided"};
  }
}

namespace Xi {
template <>
Xi::Http::AuthenticationType lexical_cast<Xi::Http::AuthenticationType>(const std::string &value) {
  if (value == to_string(Xi::Http::AuthenticationType::Basic))
    return Xi::Http::AuthenticationType::Basic;
  else
    return Xi::Http::AuthenticationType::Unsupported;
}
}  // namespace Xi
