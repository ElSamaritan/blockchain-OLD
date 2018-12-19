#include "Xi/Http/FunctorRequestHandler.h"

Xi::Http::FunctorRequestHandler::FunctorRequestHandler(Xi::Http::FunctorRequestHandler::function_t functor)
    : m_functor{functor} {}

Xi::Http::Response Xi::Http::FunctorRequestHandler::doHandleRequest(const Xi::Http::Request &request) {
  return m_functor(request);
}
