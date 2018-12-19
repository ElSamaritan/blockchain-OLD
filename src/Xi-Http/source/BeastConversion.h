#pragma once

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/optional/optional_io.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include "Xi/Http/Request.h"
#include "Xi/Http/Response.h"

namespace Xi {
namespace Http {
struct BeastConversion {
  using beast_request_t = boost::beast::http::request<boost::beast::http::string_body>;
  using beast_response_t = boost::beast::http::response<boost::beast::http::string_body>;

  using api_request_t = Request;
  using api_response_t = Response;

  api_request_t operator()(const beast_request_t& request) const;
  api_response_t operator()(const beast_response_t& response) const;

  beast_request_t operator()(const api_request_t& request) const;
  beast_response_t operator()(const api_response_t& response) const;

  void copyHeaders(const HeaderContainer& headers, beast_request_t& request) const;
  void copyHeaders(const HeaderContainer& headers, beast_response_t& response) const;
  void copyHeaders(const beast_request_t& request, HeaderContainer& headers) const;
  void copyHeaders(const beast_response_t& response, HeaderContainer& headers) const;

  std::string decodeBody(boost::iostreams::array_source source, boost::optional<ContentEncoding> encoding) const;
  std::string encodeBody(boost::iostreams::array_source source, boost::optional<ContentEncoding> encoding) const;
};
}  // namespace Http
}  // namespace Xi
