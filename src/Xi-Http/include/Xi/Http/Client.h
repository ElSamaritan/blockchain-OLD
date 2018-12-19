#pragma once

#include <cinttypes>
#include <string>
#include <vector>
#include <future>

#include <System/Dispatcher.h>

#include <Xi/Global.h>

#include "Xi/Http/Request.h"
#include "Xi/Http/Response.h"

namespace Xi {
namespace Http {
class ClientSession;

class Client {
 public:
  /*!
   * \brief Client creates an new http client connected to the given host.
   * \param host The hostname this clients connects to, can be an DNS entry.
   * \param port The port to connect to if necessary, if 0 then the default port is used (HTTP: 80, HTTPS: 443)
   */
  explicit Client(const std::string& host, uint16_t port, bool ssl = true);
  XI_DELETE_COPY(Client);
  XI_DEFAULT_MOVE(Client);
  virtual ~Client();

  const std::string host() const;
  uint16_t port() const;

  /*!
   * \brief ssl indicates if ssl connections are required
   */
  bool ssl() const;

  /*!
   * \brief setSSL sets if ssl connections are required
   *
   * If ssl is required every request going to this client will make an ssl request and will not accept any retargeting
   * to non ssl secured hosts.
   */
  void setSSL(bool m_ssl);

  /*!
   * \brief send sends the request to the server asynchroniously
   *
   * \throws std::runtime_error if the session pool is exhausted
   */
  std::future<Response> send(Request&& request);
  std::future<Response> send(const std::string& url, Method method, ContentType type, const std::string& body = "");

#define MAKE_METHOD_ALIAS(NAME, METHOD)                                                                       \
  inline std::future<Response> NAME(const std::string& url, ContentType type, const std::string& body = "") { \
    return send(url, Method::METHOD, type, body);                                                             \
  }                                                                                                           \
  inline Response NAME##Sync(const std::string& url, ContentType type, const std::string& body = "") {        \
    return send(url, Method::METHOD, type, body).get();                                                       \
  }

  MAKE_METHOD_ALIAS(get, Get)
  MAKE_METHOD_ALIAS(post, Post)
  MAKE_METHOD_ALIAS(del, Delete)
  MAKE_METHOD_ALIAS(put, Put)
  MAKE_METHOD_ALIAS(head, Head)

#undef MAKE_METHOD_ALIAS

 private:
  const std::string m_host;
  const uint16_t m_port;
  bool m_ssl;

  struct _Worker;
  std::shared_ptr<_Worker> m_worker;
};
}  // namespace Http
}  // namespace Xi
