#pragma once

#include <string>

#include "Xi/Http/Request.h"
#include "Xi/Http/Response.h"

namespace Xi {
namespace Http {
class RequestHandler {
 public:
  RequestHandler() = default;
  virtual ~RequestHandler() = default;

  Response operator()(const Request& request);

  Response makeBadRequest(const std::string& why = "");
  Response makeNotFound(const std::string& why = "");
  Response makeInternalServerError(const std::string& why = "");

 protected:
  virtual Response doHandleRequest(const Request& request) = 0;

  virtual Response doMakeBadRequest(const std::string& why);
  virtual Response doMakeNotFound(const std::string& why);
  virtual Response doMakeInternalServerError(const std::string& why);

 private:
  Response fail(std::exception_ptr ex);
};
}  // namespace Http
}  // namespace Xi
