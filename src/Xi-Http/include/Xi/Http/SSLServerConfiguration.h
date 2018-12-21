#pragma once

#include <string>

namespace boost {
namespace asio {
namespace ssl {
class context;
}
}  // namespace asio
}  // namespace boost

namespace Xi {
namespace Http {
struct SSLServerConfiguration {
  bool Disabled;

  std::string CertFile;
  std::string KeyFile;
  std::string Password;
  std::string DhFile;

  SSLServerConfiguration();
  ~SSLServerConfiguration() = default;

  void initializeContext(boost::asio::ssl::context& ctx);
};
}  // namespace Http
}  // namespace Xi
