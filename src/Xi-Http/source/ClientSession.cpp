#include "ClientSession.h"

#include <stdexcept>
#include <sstream>
#include <cinttypes>

#include <Xi/Utils/ExternalIncludePush.h>
#include <zlib.h>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Utils/String.h>

#include "Xi/Http/Uri.h"

#include "HttpClientSession.h"

Xi::Http::ClientSession::ClientSession(boost::asio::io_context &io, IClientSessionBuilder &builder)
    : m_io{io}, m_resolver{io}, m_builder{builder}, m_isRedirected{false} {}

Xi::Http::ClientSession::future_t Xi::Http::ClientSession::run(const char *_host, const char *port,
                                                               Xi::Http::Request &&request) {
  try {
    request.headers().setAcceptedContentEncodings(
        {ContentEncoding::Gzip, ContentEncoding::Deflate, ContentEncoding::Identity});
    m_originalRequest = std::move(request);
    m_host = _host;
    m_port = port;
    run();
    return m_promise.get_future();
  } catch (...) {
    throw;
  }
}

void Xi::Http::ClientSession::onHostResolved(boost::beast::error_code ec, resolver_t::results_type results) {
  try {
    checkErrorCode(ec);
    doOnHostResolved(std::move(results));
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
    decodeResponse();
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
    finalize();
  } catch (...) {
    fail(std::current_exception());
  }
}

const char *Xi::Http::ClientSession::host() { return m_host.c_str(); }

void Xi::Http::ClientSession::finalize() {
  if (!m_isRedirected) m_promise.set_value(std::move(m_responseToEmit));
  m_selfHoldingPointer.reset();
  fail(std::current_exception());
}

void Xi::Http::ClientSession::swallowErrorCode(const boost::beast::error_code &ec) { XI_UNUSED(ec); }

void Xi::Http::ClientSession::checkErrorCode(const boost::beast::error_code &ec) {
  if (ec) throw std::runtime_error{ec.message()};
}

void Xi::Http::ClientSession::decodeResponse() {
  m_responseToEmit.setStatus(static_cast<StatusCode>(m_response.result()));
  copyHeaders();

  if (m_responseToEmit.isRedirection())
    redirect();
  else {
    auto potentialEncoding = m_responseToEmit.headers().contentEncoding();
    auto encoding = potentialEncoding ? *potentialEncoding : ContentEncoding::Identity;

    boost::iostreams::array_source source{m_response.body().data(), m_response.body().size()};
    boost::iostreams::filtering_istream filter;

    if (encoding == ContentEncoding::Deflate)
      filter.push(boost::iostreams::zlib_decompressor{-MAX_WBITS});
    else if (encoding == ContentEncoding::Gzip)
      filter.push(boost::iostreams::gzip_decompressor{});
    else if (encoding != ContentEncoding::Identity)
      throw std::runtime_error{"content was encoded with unsupported compression"};

    filter.push(source);

    boost::iostreams::filtering_streambuf<boost::iostreams::output> out(m_bodyDecodingBuffer);
    boost::iostreams::copy(filter, out);
    m_responseToEmit.setBody(m_bodyDecodingBuffer.str());
  }
}

void Xi::Http::ClientSession::fail(const std::exception_ptr &ex) {
  m_promise.set_exception(ex);
  m_selfHoldingPointer.reset();
}

void Xi::Http::ClientSession::run() {
  try {
    m_selfHoldingPointer = shared_from_this();
    copyHeaders(m_originalRequest);
    m_request.set(boost::beast::http::field::host, m_host.c_str());
    m_request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);  // TODO Xi-Version
    m_request.set(boost::beast::http::field::connection, "close");
    m_request.target(m_originalRequest.url().c_str());
    m_request.version(static_cast<uint32_t>(m_originalRequest.version()));
    m_request.method(static_cast<boost::beast::http::verb>(m_originalRequest.method()));
    m_request.body() = m_originalRequest.body();

    doPrepareRun();

    m_resolver.async_resolve(
        m_host.c_str(), m_port.c_str(),
        std::bind(&ClientSession::onHostResolved, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
  } catch (...) {
    m_selfHoldingPointer.reset();
    throw;
  }
}

void Xi::Http::ClientSession::redirect() {
  const auto location = m_responseToEmit.headers().location();
  if (!location)
    throw std::runtime_error{"recieved redirection wihtout given location"};
  else if (m_responseToEmit.redirections() > 4)
    throw std::runtime_error{"redirection limit reached"};
  else {
    bool isSSLRequired = m_originalRequest.isSSLRequired();
    const Uri uri{*location};
    if (isSSLRequired && uri.scheme() == "http")
      throw std::runtime_error{"connection tried to link to none ssl encrypted endpoint"};
    else if (uri.scheme() == "https")
      isSSLRequired = true;

    auto redirectedSession = isSSLRequired ? m_builder.makeHttpsSession() : m_builder.makeHttpSession();
    redirectedSession->m_originalRequest = std::move(m_originalRequest);
    redirectedSession->m_promise = std::move(m_promise);

    // Determine possible port redirection
    if (uri.port())
      redirectedSession->m_port = std::to_string(uri.port());
    else if (uri.scheme() == "http")
      redirectedSession->m_port = std::to_string(80);
    else if (uri.scheme() == "https")
      redirectedSession->m_port = std::to_string(443);
    else
      redirectedSession->m_port = m_port;

    if (!uri.host().empty())
      redirectedSession->m_host = uri.host();
    else
      redirectedSession->m_host = m_host;

    if (!uri.username().empty())
      redirectedSession->m_originalRequest.headers().setBasicAuthorization(uri.username(), uri.password());

    std::string uriLocation = std::string{"/"} + uri.path();
    if (!uri.query().empty()) uriLocation += std::string{"?"} + uri.query();
    if (!uri.fragment().empty()) uriLocation += std::string{"#"} + uri.fragment();
    redirectedSession->m_originalRequest.setUrl(uriLocation);

    redirectedSession->run();
    m_isRedirected = true;
  }
}

void Xi::Http::ClientSession::copyHeader(Xi::Http::HeaderContainer::Header header) {
  auto search = m_response.find(static_cast<boost::beast::http::field>(header));
  if (search != m_response.end()) m_responseToEmit.headers().set(header, std::string{search->value()});
}

void Xi::Http::ClientSession::copyHeaders() {
  copyHeader(HeaderContainer::WWWAuthenticate);
  copyHeader(HeaderContainer::Authorization);
  copyHeader(HeaderContainer::Connection);
  copyHeader(HeaderContainer::KeepAlive);
  copyHeader(HeaderContainer::Accept);
  copyHeader(HeaderContainer::AcceptCharset);
  copyHeader(HeaderContainer::AcceptEncoding);
  copyHeader(HeaderContainer::ContentType);
  copyHeader(HeaderContainer::ContentEncoding);
  copyHeader(HeaderContainer::Location);
  copyHeader(HeaderContainer::Allow);
  copyHeader(HeaderContainer::Server);
}

void Xi::Http::ClientSession::copyHeaders(const Xi::Http::Request &request) {
  for (const auto &header : request.headers())
    m_request.set(static_cast<boost::beast::http::field>(header.first), header.second);
}
