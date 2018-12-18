#include "Xi/Http/ContentType.h"

#include <stdexcept>

std::string Xi::to_string(Xi::Http::ContentType status) {
  switch (status) {
    case Http::ContentType::Json:
      return "application/json";
    default:
      throw std::runtime_error{"unknow content type could not be parsed into a string representation"};
  }
}

namespace Xi {
template <>
Xi::Http::ContentType lexical_cast<Xi::Http::ContentType>(const std::string &value) {
  if (value == to_string(Http::ContentType::Json))
    return Http::ContentType::Json;
  else
    throw std::runtime_error{"unable to find a corresponding content type for the provided string"};
}
}  // namespace Xi
