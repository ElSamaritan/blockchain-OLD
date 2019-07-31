/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
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
      m_verifyPeers{true},
      m_rootPath{"./ssl"},
      m_certificatePath{"cert.pem"},
      m_privateKeyPath{"key.pem"},
      m_dhparamPath{"dh.pem"},
      m_trustedKeysPath{"trusted.pem"},
      m_privateKeyPassword{""} {
}

bool Xi::Http::SSLConfiguration::enabled() const {
  return m_enabled;
}
bool Xi::Http::SSLConfiguration::disabled() const {
  return !m_enabled;
}
void Xi::Http::SSLConfiguration::setEnabled(bool isEnabled) {
  m_enabled = isEnabled;
}

bool Xi::Http::SSLConfiguration::verifyPeers() const {
  return m_verifyPeers;
}

void Xi::Http::SSLConfiguration::setVerifyPeers(bool isEnabled) {
  m_verifyPeers = isEnabled;
}

const std::string &Xi::Http::SSLConfiguration::rootPath() const {
  return m_rootPath;
}

void Xi::Http::SSLConfiguration::setRootPath(const std::string &path) {
  m_rootPath = path;
}

const std::string &Xi::Http::SSLConfiguration::certificatePath() const {
  return m_certificatePath;
}
void Xi::Http::SSLConfiguration::setCertificatePath(const std::string &path) {
  m_certificatePath = path;
}

const std::string &Xi::Http::SSLConfiguration::privateKeyPath() const {
  return m_privateKeyPath;
}
void Xi::Http::SSLConfiguration::setPrivateKeyPath(const std::string &path) {
  m_privateKeyPath = path;
}

const std::string &Xi::Http::SSLConfiguration::dhparamPath() const {
  return m_dhparamPath;
}
void Xi::Http::SSLConfiguration::setDhparamPath(const std::string &path) {
  m_dhparamPath = path;
}

const std::string &Xi::Http::SSLConfiguration::trustedKeysPath() const {
  return m_trustedKeysPath;
}
void Xi::Http::SSLConfiguration::setTrustedKeysPath(const std::string &path) {
  m_trustedKeysPath = path;
}

const std::string &Xi::Http::SSLConfiguration::privateKeyPassword() const {
  return m_privateKeyPassword;
}
void Xi::Http::SSLConfiguration::setPrivateKeyPassword(const std::string &password) {
  m_privateKeyPassword = password;
}

#define CATCH_THROW_WITH_CONTEXT(INST, FILE)                                           \
  try {                                                                                \
    INST;                                                                              \
    if (ec)                                                                            \
      throw std::runtime_error{ec.message()};                                          \
  } catch (std::exception & e) {                                                       \
    throw std::runtime_error{std::string{"unable to load "} + FILE + ": " + e.what()}; \
  }

void Xi::Http::SSLConfiguration::initializeServerContext(boost::asio::ssl::context &ctx) {
  using namespace boost::filesystem;
  if (disabled())
    return;
  if (!exists(certificatePath()))
    throw std::runtime_error{"CertFile could not be found."};
  if (!exists(privateKeyPath()))
    throw std::runtime_error{"KeyFile could not be found."};
  if (!exists(dhparamPath()))
    throw std::runtime_error{"DhFile could not be found."};

  ctx.set_password_callback([pkp = privateKeyPassword()](
                                std::size_t, boost::asio::ssl::context_base::password_purpose purpose) -> std::string {
    XI_RETURN_EC_IF(purpose != boost::asio::ssl::context_base::password_purpose::for_reading, "");
    XI_RETURN_SC(pkp);
  });

  ctx.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2);

  boost::system::error_code ec;
  CATCH_THROW_WITH_CONTEXT(ctx.use_certificate_chain_file(certificatePath(), ec), certificatePath())
  CATCH_THROW_WITH_CONTEXT(ctx.use_private_key_file(privateKeyPath(), boost::asio::ssl::context::file_format::pem, ec),
                           privateKeyPath())
  CATCH_THROW_WITH_CONTEXT(ctx.use_tmp_dh_file(dhparamPath(), ec), dhparamPath())
}

void Xi::Http::SSLConfiguration::initializeClientContext(boost::asio::ssl::context &ctx) {
  using namespace boost::filesystem;

  if (disabled() || !verifyPeers()) {
    ctx.set_verify_mode(boost::asio::ssl::verify_none);
  } else {
    ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    boost::system::error_code ec;
    CATCH_THROW_WITH_CONTEXT(ctx.load_verify_file(trustedKeysPath(), ec), trustedKeysPath())
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
