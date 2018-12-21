#include "Xi/Http/SSLClientConfiguration.h"

#include <fstream>
#include <streambuf>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/filesystem.hpp>
#include <boost/asio/ssl.hpp>
#include <nlohmann/json.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

namespace {
const std::string CLIPrefix{"ssl-client-"};
const std::string DisableSSLName{"disable"};
const std::string VerifyPeersName{"disable-peer-verification"};
const std::string TrustedFileName{"certs-bundle"};
}  // namespace

Xi::Http::SSLClientConfiguration::SSLClientConfiguration() {
  Disabled = false;
  VerifyPeers = true;
  TrustedFile = "./certs/trusted.pem";
}

void Xi::Http::SSLClientConfiguration::initializeContext(boost::asio::ssl::context &ctx) {
  using namespace boost::filesystem;

  if (Disabled) {
    ctx.set_verify_mode(boost::asio::ssl::verify_none);
    return;
  }
  ctx.set_verify_mode(VerifyPeers ? boost::asio::ssl::verify_peer : boost::asio::ssl::verify_none);

  if (VerifyPeers) {
    boost::system::error_code ec;
    ctx.load_verify_file(TrustedFile, ec);
    if (ec) throw std::runtime_error{"unable to load trusted peers certificates"};
  }
}

void Xi::Http::SSLClientConfiguration::emplaceCLIConfiguration(cxxopts::Options &opts) {
  // clang-format off
  opts.add_options("SSL-Client")
      (CLIPrefix + DisableSSLName, "Disables the ssl capabilities of the client. This is very unsafe and can result in "
                      "Man in the Middle attacks and expose private information.",
       cxxopts::value<bool>(Disabled)->implicit_value("true")->default_value("false"))

      (CLIPrefix + VerifyPeersName, "Disables the certificate check to only connect to clients you trust. "
                                    "This is very unsafe and can result in Man in the Middle attacks and expose "
                                    "private information.",
       cxxopts::value<bool>(VerifyPeers)->implicit_value("false")->default_value("true"))

      (CLIPrefix + TrustedFileName, "Used to search for trusted certificates in order to verify peers you are connecting to.",
       cxxopts::value<std::string>(TrustedFile)->default_value("./certs/trusted.pem"));
  // clang-format on
}

void Xi::Http::SSLClientConfiguration::load(const nlohmann::json &json) {
  if (json.find(DisableSSLName) != json.end()) {
    Disabled = json[DisableSSLName].get<bool>();
  }
  if (json.find(VerifyPeersName) != json.end()) {
    VerifyPeers = json[VerifyPeersName].get<bool>();
  }
  if (json.find(TrustedFileName) != json.end()) {
    TrustedFile = json[TrustedFileName].get<std::string>();
  }
}

void Xi::Http::SSLClientConfiguration::store(nlohmann::json &json) const {
  json[DisableSSLName] = Disabled;
  json[VerifyPeersName] = VerifyPeers;
  json[TrustedFileName] = TrustedFile;
}

bool Xi::Http::SSLClientConfiguration::isInsecure() const { return Disabled || !VerifyPeers; }
