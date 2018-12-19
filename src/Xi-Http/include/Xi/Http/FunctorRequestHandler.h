#pragma once

#include <functional>

#include <Xi/Global.h>

#include "Xi/Http/RequestHandler.h"

namespace Xi {
namespace Http {
/*!
 * \brief The FunctorRequestHandler class handles request by forwarding it to a functor object
 */
class FunctorRequestHandler : public RequestHandler {
 public:
  using function_t = std::function<Response(const Request&)>;

 public:
  /*!
   * \brief FunctorRequestHandler constructs a request handler
   * \param functor the function object that should be called to server the request.
   */
  FunctorRequestHandler(function_t functor);
  XI_DEFAULT_COPY(FunctorRequestHandler);
  XI_DEFAULT_MOVE(FunctorRequestHandler);
  virtual ~FunctorRequestHandler() override = default;

 protected:
  /*!
   * \brief doHandleRequest forwards the request to the functor provided
   */
  Response doHandleRequest(const Request& request) override;

 private:
  function_t m_functor;
};
}  // namespace Http
}  // namespace Xi
