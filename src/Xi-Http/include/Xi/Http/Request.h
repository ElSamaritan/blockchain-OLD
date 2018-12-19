#pragma once

#include <Xi/Global.h>

#include "Xi/Http/Method.h"
#include "Xi/Http/Version.h"
#include "Xi/Http/HeaderContainer.h"

namespace Xi {
namespace Http {
/*!
 * \brief The Request class is a wrapper for boost:beast requests with limited functionality to server xi.
 */
class Request final {
  XI_DELETE_COPY(Request);

 public:
  Request();
  Request(const std::string& target, Method method = Method::Get);
  XI_DEFAULT_MOVE(Request);
  ~Request() = default;

  /*!
   * \brief url is the url to query.
   */
  const std::string& target() const;

  /*!
   * \brief setUrl sets the url to query.
   */
  void setTarget(const std::string& target);

  /*!
   * \brief method is the specified method that will be used once emitting the request. Default: Get
   */
  Method method() const;

  /*!
   * \brief setMethod sets the method to be used accoring to the HTTP protocol. Default: Get
   */
  void setMethod(Method method);

  /*!
   * \brief version is the http protocol version that should be used. Default: v1.1
   */
  Version version() const;

  /*!
   * \brief setVersion sets the http protocol version to be used. Default v1.1
   */
  void setVersion(Version version);

  /*!
   * \brief body is the request body that will be send to the enpoint once the request is emitted.
   */
  const std::string& body() const;

  /*! * \brief setBody sets the request body that will be send to the ecpoint once the request is emitted.
   */
  void setBody(const std::string& body);

  /*!
   * \brief setBody sets the request body that will be send to the ecpoint once the request is emitted.
   */
  void setBody(std::string&& body);

  /*!
   * \brief isSSLRequired returns true if ssl is required by the client
   *
   * A request may get redirected during it evaluation and the redirected endpoint may not offer ssl encryption, thus
   * the request must abort a connection once its get redirected to such endpoint if this flag is set.
   */
  bool isSSLRequired() const;
  void setSSLRequired(bool isRequired);

  HeaderContainer& headers();
  const HeaderContainer& headers() const;

  const std::string& host() const;
  void setHost(const std::string& host);

  uint16_t port() const;
  void setPort(uint16_t port);

 private:
  friend class Server;

  HeaderContainer m_headers;
  std::string m_body;
  Version m_version;
  Method m_method;
  std::string m_target;
  std::string m_host;
  uint16_t m_port;
  bool m_sslRequired;
};
}  // namespace Http
}  // namespace Xi
