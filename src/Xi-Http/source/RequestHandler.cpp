#include "Xi/Http/RequestHandler.h"

#include <Xi/Global.h>

Xi::Http::Response Xi::Http::RequestHandler::operator()(const Xi::Http::Request &request) {
  try {
    return doHandleRequest(request);
  } catch (...) {
    try {
      return fail(std::current_exception());
    } catch (...) {
      // TODO logging
      Response response{};
      response.setStatus(StatusCode::InternalServerError);
      return response;
    }
  }
}

Xi::Http::Response Xi::Http::RequestHandler::makeBadRequest(const std::string &why) { return doMakeBadRequest(why); }

Xi::Http::Response Xi::Http::RequestHandler::makeNotFound(const std::string &why) { return doMakeNotFound(why); }

Xi::Http::Response Xi::Http::RequestHandler::makeNotImplemented() { return doMakeNotImplemented(); }

Xi::Http::Response Xi::Http::RequestHandler::makeInternalServerError(const std::string &why) {
  return doMakeInternalServerError(why);
}

Xi::Http::Response Xi::Http::RequestHandler::doMakeBadRequest(const std::string &why) {
  return Response{StatusCode::BadRequest, why};
}

Xi::Http::Response Xi::Http::RequestHandler::doMakeNotFound(const std::string &why) {
  return Response{StatusCode::NotFound, why};
}

Xi::Http::Response Xi::Http::RequestHandler::doMakeNotImplemented() { return Response{StatusCode::NotImplemented}; }

Xi::Http::Response Xi::Http::RequestHandler::doMakeInternalServerError(const std::string &why) {
  return Response{StatusCode::InternalServerError, why};
}

Xi::Http::Response Xi::Http::RequestHandler::fail(std::exception_ptr ex) {
  XI_UNUSED(ex);
  return makeInternalServerError("");
}
