#include "Xi/Http/Request.h"

Xi::Http::Request::Request() : m_headers{}, m_body{}, m_version{Version::Http1_1}, m_method{Method::Get}, m_url{"/"} {}

Xi::Http::Request::Request(const std::string &url, Xi::Http::Method method) : Request() {
  setUrl(url);
  setMethod(method);
}

const std::string &Xi::Http::Request::url() const { return m_url; }

void Xi::Http::Request::setUrl(const std::string &url) { m_url = url.empty() ? "/" : url; }

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
