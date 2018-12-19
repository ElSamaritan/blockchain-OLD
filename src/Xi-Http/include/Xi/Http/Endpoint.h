#pragma once

#include <map>
#include <string>

#include <Xi/Global.h>

#include "Xi/Http/RequestHandler.h"

namespace Xi {
namespace Http {
/*!
 * \brief The Endpoint class conditionally handles requests
 *
 * An endpoint is a request handler that only acts to applicable request, thus a router can determine which endpoint
 * to choose based on this interface.
 */
class Endpoint : public RequestHandler {
 public:
  Endpoint() = default;
  XI_DEFAULT_COPY(Endpoint);
  XI_DEFAULT_MOVE(Endpoint);
  virtual ~Endpoint() override = default;

  /*!
   * \brief acceptsRequest check if the request can be handled by this endpoint
   * \param request the request to check
   * \return true if the endpoint is willing to handle this request otherwise false
   */
  virtual bool acceptsRequest(const Request& request) = 0;
};
}  // namespace Http
}  // namespace Xi
