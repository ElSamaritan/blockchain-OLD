#pragma once

#include <string>

#include <Xi/Utils/String.h>

namespace Xi {
namespace Http {
/*!
 * The Content-Encoding entity header is used to compress the media-type. When present, its value indicates which
 * encodings were applied to the entity-body. It lets the client know how to decode in order to obtain the media-type
 * referenced by the Content-Type header.
 *
 * The recommendation is to compress data as much as possible and therefore to use this field, but some types of
 * resources, such as jpeg images, are already compressed. Sometimes, using additional compression doesn't reduce
 * payload size and can even make the payload longer.
 *
 * Documentation is taken from https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Encoding
 */
enum struct ContentEncoding {
  Gzip,  ///< A format using the Lempel-Ziv coding (LZ77), with a 32-bit CRC. This is the original format of the UNIX
         ///< gzip program. The HTTP/1.1 standard also recommends that the servers supporting this content-encoding
         ///< should recognize x-gzip as an alias, for compatibility purposes.
  Compress,  ///< A format using the Lempel-Ziv-Welch (LZW) algorithm. The value name was taken from the UNIX compress
             ///< program, which implemented this algorithm. Like the compress program, which has disappeared from most
             ///< UNIX distributions, this content-encoding is not used by many browsers today, partly because of a
             ///< patent issue (it expired in 2003).
  Deflate,   ///< Using the zlib structure (defined in RFC 1950) with the deflate compression algorithm (defined in RFC
             ///< 1951).
  Identity,  ///< Indicates the identity function (i.e., no compression or modification). This token, except if
             ///< explicitly specified, is always deemed acceptable.
  Brotli     ///< A format using the Brotli algorithm.
};
}  // namespace Http
std::string to_string(Http::ContentEncoding encoding);

template <>
Xi::Http::ContentEncoding lexical_cast<Xi::Http::ContentEncoding>(const std::string& value);
}  // namespace Xi
