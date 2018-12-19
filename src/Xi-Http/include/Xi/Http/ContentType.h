#pragma once

#include <string>

#include <Xi/Utils/String.h>

namespace Xi {
namespace Http {
/*!
 * \brief The Content-Type entity header is used to indicate the media type of the resource.
 *
 * In responses, a Content-Type header tells the client what the content type of the returned content actually is.
 * Browsers will do MIME sniffing in some cases and will not necessarily follow the value of this header; to prevent
 * this behavior, the header X-Content-Type-Options can be set to nosniff.
 *
 * In requests, (such as POST or PUT), the client tells the server what type of data is actually sent.
 *
 * Documentation is taken from https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Type
 */
enum struct ContentType { Html, Plain, Xml, Json, Text };
}  // namespace Http
std::string to_string(Http::ContentType status);

template <>
Xi::Http::ContentType lexical_cast<Xi::Http::ContentType>(const std::string& value);
}  // namespace Xi
