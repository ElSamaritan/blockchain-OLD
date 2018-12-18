#include "Xi/Http/Response.h"

#include <cinttypes>

Xi::Http::Response::Response() : m_headers{}, m_body{}, m_statusCode{StatusCode::Ok}, m_redirections{0} {}

Xi::Http::StatusCode Xi::Http::Response::status() const { return m_statusCode; }

void Xi::Http::Response::setStatus(Xi::Http::StatusCode code) { m_statusCode = code; }

const std::string &Xi::Http::Response::body() const { return m_body; }

void Xi::Http::Response::setBody(const std::string &_body) {
  std::string movableBody{_body};
  setBody(std::move(movableBody));
}

void Xi::Http::Response::setBody(std::string &&_body) { m_body = std::move(_body); }

bool Xi::Http::Response::isRedirection() const { return (static_cast<uint32_t>(status()) / 100) == 3; }

Xi::Http::HeaderContainer &Xi::Http::Response::headers() { return m_headers; }

const Xi::Http::HeaderContainer &Xi::Http::Response::headers() const { return m_headers; }

uint16_t Xi::Http::Response::redirections() const { return m_redirections; }

void Xi::Http::Response::setRedirections(uint16_t num) { m_redirections = num; }
