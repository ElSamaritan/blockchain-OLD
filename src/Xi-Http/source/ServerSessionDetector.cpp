#include "ServerSessionDetector.h"

#include <stdexcept>

#include <Xi/Utils/ExternalIncludePush.h>
#include "DetectSSL.h"
#include <Xi/Utils/ExternalIncludePop.h>

#include "HttpServerSession.h"
#include "HttpsServerSession.h"

Xi::Http::ServerSessionDetector::ServerSessionDetector(boost::asio::ip::tcp::socket socket,
                                                       boost::asio::ssl::context &ctx,
                                                       std::shared_ptr<IServerSessionBuilder> builder)
    : m_socket{std::move(socket)}, m_ctx{ctx}, m_strand{m_socket.get_executor()}, m_builder{builder} {
  if (m_builder.get() == nullptr) throw std::invalid_argument{"builder cannot be a nullptr"};
}

void Xi::Http::ServerSessionDetector::run() {
  async_detect_ssl(m_socket, m_buffer,
                   boost::asio::bind_executor(m_strand, std::bind(&ServerSessionDetector::onDetect, shared_from_this(),
                                                                  std::placeholders::_1, std::placeholders::_2)));
}

void Xi::Http::ServerSessionDetector::onDetect(boost::beast::error_code ec, boost::logic::tribool result) {
  if (ec) {
    // TODO Logging
  }

  if (result) {
    return m_builder->makeHttpsServerSession(std::move(m_socket), std::move(m_buffer))->run();
  } else {
    return m_builder->makeHttpServerSession(std::move(m_socket), std::move(m_buffer))->run();
  }
}
