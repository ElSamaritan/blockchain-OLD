#include "Xi/Http/ContentEncoding.h"

#include <stdexcept>

std::string Xi::to_string(Xi::Http::ContentEncoding encoding) {
  switch (encoding) {
    case Http::ContentEncoding::Gzip:
      return "gzip";
    case Http::ContentEncoding::Compress:
      return "compress";
    case Http::ContentEncoding::Deflate:
      return "deflate";
    case Http::ContentEncoding::Identity:
      return "identity";
    case Http::ContentEncoding::Brotli:
      return "br";
    default:
      throw std::runtime_error{"unknown content encoding passed to to_string"};
  }
}

namespace Xi {
template <>
Http::ContentEncoding lexical_cast<Http::ContentEncoding>(const std::string& str) {
  if (str == to_string(Http::ContentEncoding::Gzip))
    return Http::ContentEncoding::Gzip;
  else if (str == to_string(Http::ContentEncoding::Compress))
    return Http::ContentEncoding::Compress;
  else if (str == to_string(Http::ContentEncoding::Deflate))
    return Http::ContentEncoding::Deflate;
  else if (str == to_string(Http::ContentEncoding::Identity))
    return Http::ContentEncoding::Identity;
  else if (str == to_string(Http::ContentEncoding::Brotli))
    return Http::ContentEncoding::Brotli;
  else
    throw std::runtime_error{"unexpected string representation of content encoding"};
}
}  // namespace Xi
