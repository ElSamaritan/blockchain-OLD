#include "Xi/Http/BasicEndpoint.h"

#include <stdexcept>

#include <Xi/Utils/String.h>

Xi::Http::BasicEndpoint::BasicEndpoint(const std::string &target, Xi::Http::Method method,
                                       std::shared_ptr<Xi::Http::RequestHandler> handler)
    : m_expectedTarget{target}, m_expectedMethod{method}, m_handler{handler} {}

bool Xi::Http::BasicEndpoint::acceptsRequest(const Xi::Http::Request &request) {
  return request.method() == m_expectedMethod && request.target() == m_expectedTarget;
}

Xi::Http::Response Xi::Http::BasicEndpoint::doHandleRequest(const Xi::Http::Request &request) {
  if (!acceptsRequest(request) || m_handler.get() == nullptr) return makeNotImplemented();
  return m_handler->operator()(request);
}
