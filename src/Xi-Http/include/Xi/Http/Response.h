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
  XI_DELETE_COPY(Response);

 public:
  XI_DEFAULT_MOVE(Response);

  Response();
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

  uint16_t redirections() const;
  void setRedirections(uint16_t num);

 private:
  friend class Client;
  friend class Session;

  HeaderContainer m_headers;
  std::string m_body;
  StatusCode m_statusCode;
  uint16_t m_redirections;
};
}  // namespace Http
}  // namespace Xi
