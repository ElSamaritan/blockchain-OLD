#pragma once

#include <string>
#include <memory>

#include <Xi/Global.h>

#include "Xi/Http/Endpoint.h"
#include "Xi/Http/Method.h"

namespace Xi {
namespace Http {
/*!
 * \brief The BasicEndpoint class conditionally accepts requests based on their path/method and forwards it to a handler
 */
class BasicEndpoint : public Endpoint {
 public:
  /*!
   * \brief BasicEndpoint construct a basic endpoint
   * \param target required target this endpoint represents
   * \param method the method this enpoint can handle
   * \param handler the handler to forward the request to, can be nullptr
   */
  BasicEndpoint(const std::string& target, Method method, std::shared_ptr<RequestHandler> handler);
  XI_DEFAULT_COPY(BasicEndpoint);
  XI_DEFAULT_MOVE(BasicEndpoint);
  virtual ~BasicEndpoint() override = default;

  /*!
   * \brief acceptsRequest checks if the request is applicable for this endpoint.
   * \param request the request to check
   * \return true if the target and method of this enpoint and the request are equal otherwise false
   */
  bool acceptsRequest(const Request& request) override;

 protected:
  /*!
   * \brief doHandleRequest handles the given request by forwarding it to the provided handler
   * \return notImplemented response if the request is not accepted or no handler is provided, otherwise the result of
   * the handler provided
   */
  Response doHandleRequest(const Request& request) override;

 private:
  std::string m_expectedTarget;
  Method m_expectedMethod;
  std::shared_ptr<RequestHandler> m_handler;
};
}  // namespace Http
}  // namespace Xi
