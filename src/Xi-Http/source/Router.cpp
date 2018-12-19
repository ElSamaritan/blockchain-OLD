#include "Xi/Http/Router.h"

#include <stdexcept>

void Xi::Http::Router::addEndpoint(std::shared_ptr<Xi::Http::Endpoint> endpoint, uint32_t priority) {
  m_endpoints.insert(std::make_pair(priority, endpoint));
}

bool Xi::Http::Router::acceptsRequest(const Xi::Http::Request &request) {
  for (auto it = m_endpoints.rbegin(); it != m_endpoints.rend(); ++it) {
    if (it->second->acceptsRequest(request)) return true;
  }
  return false;
}

Xi::Http::Response Xi::Http::Router::doHandleRequest(const Xi::Http::Request &request) {
  for (auto it = m_endpoints.rbegin(); it != m_endpoints.rend(); ++it) {
    if (it->second->acceptsRequest(request)) return it->second->operator()(request);
  }
  return makeNotFound();
}
