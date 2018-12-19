#include "Xi/Http/Request.h"

#include "Xi/Http/Uri.h"

Xi::Http::Request::Request()
    : m_headers{},
      m_body{},
      m_version{Version::Http1_1},
      m_method{Method::Get},
      m_target{"/"},
      m_port{0},
      m_sslRequired{false} {}

Xi::Http::Request::Request(const std::string &url, Xi::Http::Method method) : Request() {
  setTarget(url);
  setMethod(method);
}

const std::string &Xi::Http::Request::target() const { return m_target; }

void Xi::Http::Request::setTarget(const std::string &url) { url.empty() ? m_target = "/" : m_target = url; }

Xi::Http::Method Xi::Http::Request::method() const { return m_method; }

void Xi::Http::Request::setMethod(Xi::Http::Method method) { m_method = method; }

Xi::Http::Version Xi::Http::Request::version() const { return m_version; }

void Xi::Http::Request::setVersion(Xi::Http::Version version) { m_version = version; }

const std::string &Xi::Http::Request::body() const { return m_body; }

void Xi::Http::Request::setBody(const std::string &body) {
  std::string _body{body};
  setBody(std::move(_body));
}

void Xi::Http::Request::setBody(std::string &&body) { m_body = std::move(body); }

bool Xi::Http::Request::isSSLRequired() const { return m_sslRequired; }

void Xi::Http::Request::setSSLRequired(bool isRequired) { m_sslRequired = isRequired; }

Xi::Http::HeaderContainer &Xi::Http::Request::headers() { return m_headers; }

const Xi::Http::HeaderContainer &Xi::Http::Request::headers() const { return m_headers; }

const std::string &Xi::Http::Request::host() const { return m_host; }

void Xi::Http::Request::setHost(const std::string &_host) { m_host = _host; }

uint16_t Xi::Http::Request::port() const { return m_port; }

void Xi::Http::Request::setPort(uint16_t port) { m_port = port; }
