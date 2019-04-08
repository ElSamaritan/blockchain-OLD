/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

#include "Xi/Http/SSLConfiguration.h"

#include <Xi/ExternalIncludePush.h>
#include <boost/filesystem.hpp>
#include <boost/asio/ssl.hpp>
#include <Xi/ExternalIncludePop.h>

Xi::Http::SSLConfiguration::SSLConfiguration()
    : m_enabled{false},
      m_verifyPeers{false},
      m_rootPath{"./certs"},
      m_certificatePath{"cert.pem"},
      m_privateKeyPath{"key.pem"},
      m_dhparamPath{"dh.pem"},
      m_trustedKeysPath{"trusted.pem"},
      m_privateKeyPassword{""} {}

bool Xi::Http::SSLConfiguration::enabled() const { return m_enabled; }
bool Xi::Http::SSLConfiguration::disabled() const { return !m_enabled; }
void Xi::Http::SSLConfiguration::setEnabled(bool isEnabled) { m_enabled = isEnabled; }

bool Xi::Http::SSLConfiguration::verifyPeers() const { return m_verifyPeers; }

void Xi::Http::SSLConfiguration::setVerifyPeers(bool isEnabled) { m_verifyPeers = isEnabled; }

const std::string &Xi::Http::SSLConfiguration::rootPath() const { return m_rootPath; }

void Xi::Http::SSLConfiguration::setRootPath(const std::string &path) { m_rootPath = path; }

const std::string &Xi::Http::SSLConfiguration::certificatePath() const { return m_certificatePath; }
void Xi::Http::SSLConfiguration::setCertificatePath(const std::string &path) { m_certificatePath = path; }

const std::string &Xi::Http::SSLConfiguration::privateKeyPath() const { return m_privateKeyPath; }
void Xi::Http::SSLConfiguration::setPrivateKeyPath(const std::string &path) { m_privateKeyPath = path; }

const std::string &Xi::Http::SSLConfiguration::dhparamPath() const { return m_dhparamPath; }
void Xi::Http::SSLConfiguration::setDhparamPath(const std::string &path) { m_dhparamPath = path; }

const std::string &Xi::Http::SSLConfiguration::trustedKeysPath() const { return m_trustedKeysPath; }
void Xi::Http::SSLConfiguration::setTrustedKeysPath(const std::string &path) { m_trustedKeysPath = path; }

const std::string &Xi::Http::SSLConfiguration::privateKeyPassword() const { return m_privateKeyPassword; }
void Xi::Http::SSLConfiguration::setPrivateKeyPassword(const std::string &password) { m_privateKeyPassword = password; }

void Xi::Http::SSLConfiguration::serialize(CryptoNote::ISerializer &s) {
  s(m_enabled, "enabled");
  s(m_verifyPeers, "verifyPeers");
  s(m_rootPath, "directory");
  s(m_certificatePath, "certificatesPath");
  s(m_privateKeyPath, "privateKeyPath");
  s(m_dhparamPath, "dhparamPath");
  s(m_trustedKeysPath, "trustedKeysPath");
  s(m_privateKeyPassword, "privateKeyPassword");
}

void Xi::Http::SSLConfiguration::emplaceOptions(cxxopts::Options &options, Xi::Http::SSLConfiguration::Usage usage) {
  // clang-format off
  options.add_options("SSL")
        ("ssl", "Enables and enforces SSL usage to secure your privacy.",
            cxxopts::value<bool>(m_enabled)->implicit_value("true"))

        ("ssl-dir", "Directory containing ssl key files.",
            cxxopts::value<std::string>(m_rootPath)->default_value("./certs"));
  // clang-format on

  if (static_cast<int>(usage) & static_cast<int>(Usage::Server)) {
    // clang-format off
    options.add_options("SSL")
        ("ssl-cert", "Filepath containing the server certificate to use.",
            cxxopts::value<std::string>(m_certificatePath)->default_value("cert.pem"))

        ("ssl-pk", "Filepath containing the server private key for encryption.",
            cxxopts::value<std::string>(m_privateKeyPath)->default_value("key.pem"))

        ("ssl-pkp", "Password of the private key file used by the server.",
            cxxopts::value<std::string>(m_privateKeyPassword)->default_value(""))

        ("ssl-dh", "Filepath containing the dhparam randomization for the server.",
            cxxopts::value<std::string>(m_privateKeyPath)->default_value("dh.pem"));
    // clang-format on
  }

  if (static_cast<int>(usage) & static_cast<int>(Usage::Client)) {
    // clang-format off
    options.add_options("SSL")
        ("ssl-peers", "Filepath containing server certficates you want to trust.",
            cxxopts::value<std::string>(m_certificatePath)->default_value("trusted.pem"))

        ("ssl-verify", "Enables peer verification.",
            cxxopts::value<bool>(m_verifyPeers)->implicit_value("true"));
    // clang-format on
  }
}

#define CATCH_THROW_WITH_CONTEXT(INST, FILE)                         \
  try {                                                              \
    INST;                                                            \
    if (ec) throw std::runtime_error{""};                            \
  } catch (...) {                                                    \
    throw std::runtime_error{std::string{"unable to load "} + FILE}; \
  }

void Xi::Http::SSLConfiguration::initializeServerContext(boost::asio::ssl::context &ctx) {
  using namespace boost::filesystem;
  if (disabled()) return;
  if (!exists(rootedPath(certificatePath()))) throw std::runtime_error{"CertFile could not be found."};
  if (!exists(rootedPath(privateKeyPath()))) throw std::runtime_error{"KeyFile could not be found."};
  if (!exists(rootedPath(dhparamPath()))) throw std::runtime_error{"DhFile could not be found."};

  ctx.set_password_callback(
      [pkp = privateKeyPassword()](std::size_t, boost::asio::ssl::context_base::password_purpose) { return pkp; });

  ctx.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 |
                  boost::asio::ssl::context::single_dh_use);

  boost::system::error_code ec;
  CATCH_THROW_WITH_CONTEXT(ctx.use_certificate_chain_file(rootedPath(certificatePath()), ec),
                           rootedPath(certificatePath()))
  CATCH_THROW_WITH_CONTEXT(
      ctx.use_private_key_file(rootedPath(privateKeyPath()), boost::asio::ssl::context::file_format::pem, ec),
      rootedPath(privateKeyPath()))
  CATCH_THROW_WITH_CONTEXT(ctx.use_tmp_dh_file(rootedPath(dhparamPath()), ec), rootedPath(dhparamPath()))
}

void Xi::Http::SSLConfiguration::initializeClientContext(boost::asio::ssl::context &ctx) {
  using namespace boost::filesystem;

  if (disabled() || !verifyPeers()) {
    ctx.set_verify_mode(boost::asio::ssl::verify_none);
  } else {
    ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    boost::system::error_code ec;
    CATCH_THROW_WITH_CONTEXT(ctx.load_verify_file(rootedPath(trustedKeysPath()), ec), rootedPath(trustedKeysPath()))
  }
}

#undef CATCH_THROW_WITH_CONTEXT

bool Xi::Http::SSLConfiguration::isInsecure(Xi::Http::SSLConfiguration::Usage usage) const {
  if (disabled()) {
    return true;
  }

  if (static_cast<int>(usage) & static_cast<int>(Usage::Server)) {
    if (privateKeyPassword().length() < 8) {
      return true;
    }
  }

  if (static_cast<int>(usage) & static_cast<int>(Usage::Client)) {
    if (!verifyPeers()) {
      return true;
    }
  }

  return false;
}

std::string Xi::Http::SSLConfiguration::rootedPath(const std::string &path) const {
  boost::filesystem::path fpath{path};
  if (rootPath().empty() || fpath.is_absolute()) {
    return path;
  } else {
    return (boost::filesystem::path{rootPath()} / fpath).string();
  }
}
