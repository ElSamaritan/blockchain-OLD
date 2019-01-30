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

#include "ClientSession.h"

#include <stdexcept>
#include <sstream>
#include <cinttypes>

#include <Xi/Utils/ExternalIncludePush.h>
#include <openssl/ssl.h>
#include <boost/asio/ssl.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Utils/String.h>

#include "Xi/Http/Uri.h"

#include "HttpClientSession.h"

Xi::Http::ClientSession::ClientSession(boost::asio::io_context &io, std::shared_ptr<IClientSessionBuilder> builder)
    : m_io{io}, m_resolver{io}, m_redirectionCounter{0}, m_sslRequired{false}, m_builder{builder} {}

Xi::Http::ClientSession::future_t Xi::Http::ClientSession::run(Xi::Http::Request &&request) {
  m_request = m_conversion(request);
  m_port = std::to_string(static_cast<uint16_t>(request.port()));
  m_sslRequired = request.isSSLRequired();
  run();
  return m_promise.get_future();
}

void Xi::Http::ClientSession::onHostResolved(boost::beast::error_code ec, resolver_t::results_type results) {
  try {
    checkErrorCode(ec);
    doOnHostResolved(results.begin(), results.end());
  } catch (...) {
    fail(std::current_exception());
  }
}

void Xi::Http::ClientSession::onConnected(boost::beast::error_code ec) {
  try {
    checkErrorCode(ec);
    doOnConnected();
  } catch (...) {
    fail(std::current_exception());
  }
}

void Xi::Http::ClientSession::onRequestWritten(boost::beast::error_code ec, std::size_t bytesTransfered) {
  boost::ignore_unused(bytesTransfered);
  try {
    checkErrorCode(ec);
    doOnRequestWritten();
  } catch (...) {
    fail(std::current_exception());
  }
}

void Xi::Http::ClientSession::onResponseRecieved(boost::beast::error_code ec, std::size_t bytesTransfered) {
  boost::ignore_unused(bytesTransfered);
  try {
    checkErrorCode(ec);
    Response response = m_conversion(m_response);
    if (response.isRedirection()) {
      redirect(response.headers().location());
    } else
      m_promise.set_value(std::move(response));
    doOnResponseRecieved();
  } catch (...) {
    fail(std::current_exception());
  }
}

void Xi::Http::ClientSession::onShutdown(boost::beast::error_code ec) {
  try {
    (void)ec;
    if (ec == boost::asio::error::eof) {
      // Rationale:
      // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
      ec.assign(0, ec.category());
    }
    if (ec != boost::beast::errc::not_connected) swallowErrorCode(ec);
    doOnShutdown();
  } catch (...) {
    // TODO Logging
  }
}

void Xi::Http::ClientSession::swallowErrorCode(const boost::beast::error_code &ec) { XI_UNUSED(ec); }

void Xi::Http::ClientSession::checkErrorCode(const boost::beast::error_code &ec) {
  if (ec) throw std::runtime_error{ec.message()};
}

void Xi::Http::ClientSession::fail(const std::exception_ptr &ex) { m_promise.set_exception(ex); }

void Xi::Http::ClientSession::run() {
  doPrepareRun();

  const auto host = m_request.find(boost::beast::http::field::host);
  if (host == m_request.end()) throw std::runtime_error{"request has no host"};

  m_resolver.async_resolve(
      std::string{host->value()}, m_port.c_str(),
      std::bind(&ClientSession::onHostResolved, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Xi::Http::ClientSession::redirect(boost::optional<std::string> location) {
  if (m_redirectionCounter > 3)
    throw std::runtime_error{"maximum redirections reached"};
  else if (!location)
    throw std::runtime_error{"recieved redirection wihtout given location"};

  const Uri uri{*location};
  bool isSSL = m_sslRequired;
  if (!uri.host().empty()) m_request.set(boost::beast::http::field::host, uri.host());
  if (uri.port() > 0)
    m_port = std::to_string(static_cast<uint64_t>(uri.port()));
  else if (uri.scheme() == "http") {
    if (m_sslRequired)
      throw std::runtime_error{"host tried to redirect to a non ssl connection"};
    else
      m_port = std::to_string(80);
  } else if (uri.scheme() == "https") {
    isSSL = true;
    m_port = std::to_string(443);
  }
  m_request.target(uri.toString());

  if (m_builder.get() == nullptr) throw std::runtime_error{"no session builder, unable to redirect session"};

  std::shared_ptr<ClientSession> redirectedSession =
      isSSL ? m_builder->makeHttpsSession() : m_builder->makeHttpSession();
  redirectedSession->m_redirectionCounter = m_redirectionCounter + 1;
  redirectedSession->m_port = m_port;
  redirectedSession->m_sslRequired = m_sslRequired;
  redirectedSession->m_request = std::move(m_request);
  redirectedSession->m_promise = std::move(m_promise);
  redirectedSession->run();
}
