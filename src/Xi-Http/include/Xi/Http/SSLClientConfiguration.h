#pragma once

#include <string>

#include <Xi/Utils/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <nlohmann/json.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

namespace boost {
namespace asio {
namespace ssl {
class context;
}
}  // namespace asio
}  // namespace boost

namespace Xi {
namespace Http {
struct SSLClientConfiguration {
  bool Disabled;
  bool VerifyPeers;
  std::string TrustedFile;

  SSLClientConfiguration();
  ~SSLClientConfiguration() = default;

  void initializeContext(boost::asio::ssl::context& ctx);

  void emplaceCLIConfiguration(cxxopts::Options& opts);

  void load(const nlohmann::json& json);
  void store(nlohmann::json& json) const;

  bool isInsecure() const;
};
}  // namespace Http
}  // namespace Xi
