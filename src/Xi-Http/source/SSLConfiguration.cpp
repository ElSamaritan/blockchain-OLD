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

#if defined(_WIN32) && _WIN32
#include <wincrypt.h>
#endif

#include <Xi/FileSystem.h>

const Xi::Http::SSLConfiguration Xi::Http::SSLConfiguration::NoSsl{no_ssl{}};
const Xi::Http::SSLConfiguration Xi::Http::SSLConfiguration::NoVerifyClient{no_verify{}};
const Xi::Http::SSLConfiguration Xi::Http::SSLConfiguration::RootStoreClient{root_store_ssl{}};

Xi::Http::SSLConfiguration::SSLConfiguration()
    : m_enabled{false},
      m_verifyPeers{true},
      m_loadCertStore{true},
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

bool Xi::Http::SSLConfiguration::rootStoreEnabled() const {
  return m_loadCertStore;
}

void Xi::Http::SSLConfiguration::setRootStoreEnabled(bool isEnabled) {
  m_loadCertStore = isEnabled;
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

const std::vector<std::string> Xi::Http::SSLConfiguration::trustedKeys() const {
  return m_trustedKeys;
}

void Xi::Http::SSLConfiguration::addTrustedKey(const std::string &cert) {
  m_trustedKeys.push_back(cert);
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
  if (disabled())
    return;

  const auto cert = FileSystem::searchFile(certificatePath(), "pem", rootPath()).takeOrThrow();
  const auto pk = FileSystem::searchFile(privateKeyPath(), "pem", rootPath()).takeOrThrow();
  const auto dh = FileSystem::searchFile(dhparamPath(), "pem", rootPath()).takeOrThrow();

  ctx.set_password_callback([pkp = privateKeyPassword()](
                                std::size_t, boost::asio::ssl::context_base::password_purpose purpose) -> std::string {
    XI_RETURN_EC_IF(purpose != boost::asio::ssl::context_base::password_purpose::for_reading, "");
    XI_RETURN_SC(pkp);
  });

  using ssl = boost::asio::ssl::context;
  ctx.set_options(ssl::default_workarounds | ssl::no_sslv2 | ssl::no_sslv3 | ssl::no_tlsv1 | ssl::no_tlsv1_1);

  boost::system::error_code ec;
  CATCH_THROW_WITH_CONTEXT(ctx.use_certificate_chain_file(cert, ec), cert)
  CATCH_THROW_WITH_CONTEXT(ctx.use_private_key_file(pk, ssl::file_format::pem, ec), pk)
  CATCH_THROW_WITH_CONTEXT(ctx.use_tmp_dh_file(dh, ec), dh)
}

void Xi::Http::SSLConfiguration::initializeClientContext(boost::asio::ssl::context &ctx) {
  using namespace boost::filesystem;

  if (disabled() || !verifyPeers()) {
    ctx.set_verify_mode(boost::asio::ssl::verify_none);
  } else {
    ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    if (rootStoreEnabled()) {
#if defined(_WIN32) && _WIN32
      HCERTSTORE hStore = CertOpenSystemStore(0, "ROOT");
      if (hStore == nullptr) {
        return;
      }

      X509_STORE *store = X509_STORE_new();
      PCCERT_CONTEXT pContext = nullptr;
      while ((pContext = CertEnumCertificatesInStore(hStore, pContext)) != nullptr) {
        X509 *x509 = d2i_X509(nullptr, (const unsigned char **)&pContext->pbCertEncoded, pContext->cbCertEncoded);
        if (x509 != nullptr) {
          X509_STORE_add_cert(store, x509);
          X509_free(x509);
        }
      }

      CertFreeCertificateContext(pContext);
      CertCloseStore(hStore, 0);

      SSL_CTX_set_cert_store(ctx.native_handle(), store);
#else
      boost::system::error_code ec{/* */};
      ctx.set_default_verify_paths(ec);
      exceptional_if<RuntimeError>(ec.failed(), "Failed to load root certificates: {}", ec.message());
#endif
    }
    if (!trustedKeysPath().empty()) {
      boost::system::error_code ec{/* */};
      const auto tk = FileSystem::searchFile(trustedKeysPath(), "pem", rootPath()).takeOrThrow();
      CATCH_THROW_WITH_CONTEXT(ctx.load_verify_file(tk, ec), tk)
    }

    for (const auto &trustedKey : m_trustedKeys) {
      boost::system::error_code ec{/* */};
      ctx.add_certificate_authority(boost::asio::buffer(trustedKey.data(), trustedKey.size()));
      exceptional_if<RuntimeError>(ec.failed(), "Failed loading trusted key:\n{}", trustedKey);
    }
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

Xi::Http::SSLConfiguration::SSLConfiguration(Xi::Http::SSLConfiguration::root_store_ssl) : SSLConfiguration() {
  setEnabled(true);
  setVerifyPeers(true);
  setRootStoreEnabled(true);
  setTrustedKeysPath("");
}

Xi::Http::SSLConfiguration::SSLConfiguration(Xi::Http::SSLConfiguration::no_ssl) : SSLConfiguration() {
  setEnabled(false);
}

Xi::Http::SSLConfiguration::SSLConfiguration(Xi::Http::SSLConfiguration::no_verify) : SSLConfiguration() {
  setEnabled(true);
  setVerifyPeers(false);
}
