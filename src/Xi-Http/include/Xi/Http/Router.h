#pragma once

#include <map>
#include <string>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Global.h>

#include "Xi/Http/RequestHandler.h"
#include "Xi/Http/FunctorRequestHandler.h"
#include "Xi/Http/Endpoint.h"
#include "Xi/Http/BasicEndpoint.h"

namespace Xi {
namespace Http {
/*!
 * \brief The Router class routes requests to given endpoints based on the target path and method.
 */
class Router : public Endpoint {
 public:
  Router() = default;
  XI_DEFAULT_COPY(Router);
  XI_DEFAULT_MOVE(Router);
  ~Router() override = default;

  /*!
   * \brief addEndpoint adds a new endpoint to handle requests
   * \param endpoint the endpoint implementation that will be queried for incoming requests
   * \param priority determines the ordering of endpoints, endpoints with higher priority are asked first
   */
  void addEndpoint(std::shared_ptr<Endpoint> endpoint, uint32_t priority = 0);

  /*!
   * \brief acceptsRequest determines if any endpoint of the router accepts the request
   */
  bool acceptsRequest(const Request& request) override;

#define MAKE_EXPRESS_STYLE_ENDPOINT_BUILDER(METHOD)                                                                 \
  inline void on##METHOD(const std::string& target, FunctorRequestHandler::function_t handler) {                    \
    addEndpoint(                                                                                                    \
        std::make_shared<BasicEndpoint>(target, Method::METHOD, std::make_shared<FunctorRequestHandler>(handler))); \
  }                                                                                                                 \
  inline void on##METHOD(const std::string& target, std::shared_ptr<RequestHandler> handler) {                      \
    addEndpoint(std::make_shared<BasicEndpoint>(target, Method::METHOD, handler));                                  \
  }

  /*!
   * Usage:
   * \code
   * auto router = std::make_shared<Router>();
   * router.onGet("/ping", [](const auto&) { return Response{StatusCode::Ok, ""}; };
   * router.onPost("/echo", [](const auto& request) { return Response{StatusCode::Ok, request.body()}; };
   * router.onDelete("transactions", std::make_shared<TransactionDeletionHandler>(wallet));
   * \endcode
   */

  MAKE_EXPRESS_STYLE_ENDPOINT_BUILDER(Get)
  MAKE_EXPRESS_STYLE_ENDPOINT_BUILDER(Post)
  MAKE_EXPRESS_STYLE_ENDPOINT_BUILDER(Delete)
  MAKE_EXPRESS_STYLE_ENDPOINT_BUILDER(Patch)

#undef MAKE_EXPRESS_STYLE_ENDPOINT_BUILDER

 protected:
  /*!
   * \brief doHandleRequest forwards the incoming request to an endpoint or returns not found if no endpoint accepts it
   */
  Response doHandleRequest(const Request& request) override;

 private:
  std::multimap<uint32_t, std::shared_ptr<Endpoint>> m_endpoints;
};
}  // namespace Http
}  // namespace Xi
