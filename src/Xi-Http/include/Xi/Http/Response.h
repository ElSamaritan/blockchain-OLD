#pragma once

#include <string>
#include <sstream>

#include <Xi/Global.h>

#include "Xi/Http/StatusCode.h"
#include "Xi/Http/Version.h"
#include "Xi/Http/HeaderContainer.h"

namespace Xi {
namespace Http {
class Response final {
 public:
  XI_DEFAULT_MOVE(Response);
  XI_DEFAULT_COPY(Response);

  Response(StatusCode code = StatusCode::Ok, const std::string& body = "");
  ~Response() = default;

  StatusCode status() const;
  void setStatus(StatusCode code);

  const std::string& body() const;
  void setBody(const std::string& body);
  void setBody(std::string&& body);

  /*!
   * \brief isRedirect returns true iff the response encodes a redirection (3XX status).
   */
  bool isRedirection() const;

  HeaderContainer& headers();
  const HeaderContainer& headers() const;

 private:
  friend class Client;
  friend class Session;

  HeaderContainer m_headers;
  std::string m_body;
  StatusCode m_statusCode;
};
}  // namespace Http
}  // namespace Xi
