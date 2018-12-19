#include "Xi/Http/ContentType.h"

#include <stdexcept>

std::string Xi::to_string(Xi::Http::ContentType status) {
  switch (status) {
    case Http::ContentType::Html:
      return "text/html";
    case Http::ContentType::Plain:
      return "text/plain";
    case Http::ContentType::Xml:
      return "application/xml";
    case Http::ContentType::Json:
      return "application/json";
    case Http::ContentType::Text:
      return "application/text";
    default:
      throw std::runtime_error{"unknow content type could not be parsed into a string representation"};
  }
}

namespace Xi {
template <>
Xi::Http::ContentType lexical_cast<Xi::Http::ContentType>(const std::string &value) {
  if (value == to_string(Http::ContentType::Html))
    return Http::ContentType::Html;
  else if (value == to_string(Http::ContentType::Plain))
    return Http::ContentType::Plain;
  else if (value == to_string(Http::ContentType::Xml))
    return Http::ContentType::Xml;
  else if (value == to_string(Http::ContentType::Json))
    return Http::ContentType::Json;
  else if (value == to_string(Http::ContentType::Text))
    return Http::ContentType::Text;
  else
    throw std::runtime_error{"unable to find a corresponding content type for the provided string"};
}
}  // namespace Xi
