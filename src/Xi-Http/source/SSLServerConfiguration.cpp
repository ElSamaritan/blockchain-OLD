#include "Xi/Http/SSLServerConfiguration.h"

#include <string>
#include <fstream>
#include <streambuf>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/filesystem.hpp>
#include <boost/asio/ssl.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

Xi::Http::SSLServerConfiguration::SSLServerConfiguration() {
  Disabled = false;
  CertFile = "cert.pem";
  KeyFile = "key.pem";
  Password = "test";
  DhFile = "dh.pem";
}

void Xi::Http::SSLServerConfiguration::initializeContext(boost::asio::ssl::context& ctx) {
  using namespace boost::filesystem;
  if (Disabled) return;
  if (!exists(CertFile)) throw std::runtime_error{"CertFile could not be found."};
  if (!exists(KeyFile)) throw std::runtime_error{"KeyFile could not be found."};
  if (!exists(DhFile)) throw std::runtime_error{"DhFile could not be found."};

  ctx.set_password_callback(
      [Password = this->Password](std::size_t, boost::asio::ssl::context_base::password_purpose) { return Password; });

  const auto loadKey = [](const auto& filename) {
    if (!exists(filename)) throw std::runtime_error{std::string{"Unable to load '"} + filename + "' (does not exists)"};
    std::ifstream stream{filename};
    return std::string{std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
  };

  std::string cert = loadKey(CertFile);
  std::string key = loadKey(KeyFile);
  std::string dh = loadKey(DhFile);

  ctx.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 |
                  boost::asio::ssl::context::single_dh_use);

  ctx.use_certificate_chain(boost::asio::buffer(cert.data(), cert.size()));

  ctx.use_private_key(boost::asio::buffer(key.data(), key.size()), boost::asio::ssl::context::file_format::pem);

  ctx.use_tmp_dh(boost::asio::buffer(dh.data(), dh.size()));
}
