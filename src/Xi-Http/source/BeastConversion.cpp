/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

#include "BeastConversion.h"

#include <string>
#include <sstream>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Utils/String.h>

#ifndef MAX_WBITS
#define MAX_WBITS 15
#endif

namespace {
std::vector<Xi::Http::HeaderContainer::Header> headersToCopy{
    {Xi::Http::HeaderContainer::WWWAuthenticate, Xi::Http::HeaderContainer::Authorization,
     Xi::Http::HeaderContainer::Connection, Xi::Http::HeaderContainer::KeepAlive, Xi::Http::HeaderContainer::Accept,
     Xi::Http::HeaderContainer::AcceptCharset, Xi::Http::HeaderContainer::AcceptEncoding,
     Xi::Http::HeaderContainer::AccessControlAllowOrigin, Xi::Http::HeaderContainer::ContentType,
     Xi::Http::HeaderContainer::ContentEncoding, Xi::Http::HeaderContainer::Location, Xi::Http::HeaderContainer::Allow,
     Xi::Http::HeaderContainer::Server, Xi::Http::HeaderContainer::AccessControlAllowMethods}};
}

Xi::Http::BeastConversion::api_request_t Xi::Http::BeastConversion::operator()(
    const Xi::Http::BeastConversion::beast_request_t &request) const {
  api_request_t apirequest;
  copyHeaders(request, apirequest.headers());
  apirequest.setVersion(static_cast<Version>(request.version()));
  apirequest.setMethod(static_cast<Method>(request.method()));
  apirequest.setTarget(std::string{request.target()});
  apirequest.setBody(decodeBody(boost::iostreams::array_source{request.body().data(), request.body().size()},
                                apirequest.headers().contentEncoding()));
  return apirequest;
}

Xi::Http::BeastConversion::api_response_t Xi::Http::BeastConversion::operator()(
    const Xi::Http::BeastConversion::beast_response_t &response) const {
  api_response_t apiresponse;
  apiresponse.setStatus(static_cast<StatusCode>(response.result()));
  copyHeaders(response, apiresponse.headers());
  apiresponse.setBody(decodeBody(boost::iostreams::array_source{response.body().data(), response.body().size()},
                                 apiresponse.headers().contentEncoding()));
  return apiresponse;
}

Xi::Http::BeastConversion::beast_request_t Xi::Http::BeastConversion::operator()(
    const Xi::Http::BeastConversion::api_request_t &request) const {
  beast_request_t beastrequest;
  copyHeaders(request.headers(), beastrequest);
  beastrequest.set(boost::beast::http::field::host, request.host().c_str());
  beastrequest.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);  // TODO Xi-Version
  beastrequest.set(boost::beast::http::field::content_encoding, "identity");
  beastrequest.set(boost::beast::http::field::connection, "close");
  beastrequest.target(request.target().c_str());
  beastrequest.version(static_cast<uint32_t>(request.version()));
  beastrequest.method(static_cast<boost::beast::http::verb>(request.method()));
  if (!request.body().empty()) {
    beastrequest.body() = request.body();  // TODO Maybe get header first and try to send compressed body?
    beastrequest.prepare_payload();
  }
  return beastrequest;
}

Xi::Http::BeastConversion::beast_response_t Xi::Http::BeastConversion::operator()(
    const Xi::Http::BeastConversion::api_response_t &response) const {
  beast_response_t beastresponse;
  copyHeaders(response.headers(), beastresponse);
  beastresponse.result(static_cast<int>(response.status()));
  beastresponse.set(boost::beast::http::field::connection, "close");
  if (!response.body().empty()) {
    beastresponse.body() = encodeBody(boost::iostreams::array_source{response.body().data(), response.body().size()},
                                      response.headers().contentEncoding());
    beastresponse.prepare_payload();
  }
  return beastresponse;
}

void Xi::Http::BeastConversion::copyHeaders(const Xi::Http::HeaderContainer &headers,
                                            Xi::Http::BeastConversion::beast_request_t &request) const {
  for (auto header : headersToCopy) {
    const auto content = headers.get(header);
    if (content) request.set(static_cast<boost::beast::http::field>(header), content);
  }
}

void Xi::Http::BeastConversion::copyHeaders(const Xi::Http::HeaderContainer &headers,
                                            Xi::Http::BeastConversion::beast_response_t &response) const {
  for (auto header : headersToCopy) {
    const auto content = headers.get(header);
    if (content) response.set(static_cast<boost::beast::http::field>(header), content);
  }
}

void Xi::Http::BeastConversion::copyHeaders(const Xi::Http::BeastConversion::beast_request_t &request,
                                            Xi::Http::HeaderContainer &headers) const {
  for (const auto &header : headersToCopy) {
    auto search = request.find(static_cast<boost::beast::http::field>(header));
    if (search != request.end()) headers.set(header, std::string{search->value()});
  }
}

void Xi::Http::BeastConversion::copyHeaders(const Xi::Http::BeastConversion::beast_response_t &response,
                                            Xi::Http::HeaderContainer &headers) const {
  for (const auto &header : headersToCopy) {
    auto search = response.find(static_cast<boost::beast::http::field>(header));
    if (search != response.end()) headers.set(header, std::string{search->value()});
  }
}

std::string Xi::Http::BeastConversion::decodeBody(boost::iostreams::array_source source,
                                                  boost::optional<ContentEncoding> _encoding) const {
  auto encoding = _encoding ? *_encoding : ContentEncoding::Identity;
  boost::iostreams::filtering_istream filter;

  if (encoding == ContentEncoding::Deflate)
    filter.push(boost::iostreams::zlib_decompressor{-MAX_WBITS});
  else if (encoding == ContentEncoding::Gzip)
    filter.push(boost::iostreams::gzip_decompressor{});
  else if (encoding != ContentEncoding::Identity)
    throw std::runtime_error{"content was encoded with unsupported compression"};

  filter.push(source);

  std::stringstream decodingBuffer;
  boost::iostreams::filtering_streambuf<boost::iostreams::output> out(decodingBuffer);
  boost::iostreams::copy(filter, out);
  return decodingBuffer.str();
}

std::string Xi::Http::BeastConversion::encodeBody(boost::iostreams::array_source source,
                                                  boost::optional<Xi::Http::ContentEncoding> _encoding) const {
  auto encoding = _encoding ? *_encoding : ContentEncoding::Identity;
  boost::iostreams::filtering_istream filter;

  if (encoding == ContentEncoding::Deflate)
    filter.push(boost::iostreams::zlib_compressor{-MAX_WBITS});
  else if (encoding == ContentEncoding::Gzip)
    filter.push(boost::iostreams::gzip_compressor{});
  else if (encoding != ContentEncoding::Identity)
    throw std::runtime_error{"content encoding with unsupported compression"};

  filter.push(source);

  std::stringstream decodingBuffer;
  boost::iostreams::filtering_streambuf<boost::iostreams::output> out(decodingBuffer);
  boost::iostreams::copy(filter, out);
  return decodingBuffer.str();
}
