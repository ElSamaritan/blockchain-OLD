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

#pragma once

#include <string>
#include <vector>

#include <Xi/Global.h>

#include <Serialization/ISerializer.h>

#include <Xi/Utils/ExternalIncludePush.h>
#include <cxxopts.hpp>
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
class SSLConfiguration {
 public:
  /*!
   * \brief The Usage enum encodes scenerious an ssl configuration is used for.
   */
  enum struct Usage {
    Client = 1 << 0,        ///< The configuration will be used to setup a client
    Server = 1 << 1,        ///< The configuration will be used to setup a server
    Both = Client | Server  ///< The configuration will be used to setup a server and client
  };

 public:
  SSLConfiguration();
  XI_DEFAULT_COPY(SSLConfiguration);
  XI_DEFAULT_MOVE(SSLConfiguration);
  ~SSLConfiguration() = default;

  /*!
   * \brief enabled True if ssl capabilities shoud be used otherwise false
   */
  bool enabled() const;

  /*!
   * \brief disabled False if ssl capabilities shoud be used othwerise false
   */
  bool disabled() const;

  /*!
   * \brief setEnabled Turns the usage of ssl on/off
   * \param isEnabled True if ssl should be used otherwise false.
   */
  void setEnabled(bool isEnabled);

  /*!
   * \brief verifyPeers True if only verified peers should be trusted.
   */
  bool verifyPeers() const;

  /*!
   * \brief setVerifyPeers Turns checks on/off whether the connected peers is considered to be trusted
   * \param isEnabled True if connected peers should be checked agains trusted peers otherwise false
   */
  void setVerifyPeers(bool isEnabled);

  /*!
   * \brief rootPath main directory containing all keys
   */
  const std::string& rootPath() const;

  /*!
   * \brief setRootPath Sets the main directory containing all keys if relative paths are provided
   */
  void setRootPath(const std::string& path);

  const std::string& certificatePath() const;
  void setCertificatePath(const std::string& path);

  const std::string& privateKeyPath() const;
  void setPrivateKeyPath(const std::string& path);

  const std::string& dhparamPath() const;
  void setDhparamPath(const std::string& path);

  const std::string& trustedKeysPath() const;
  void setTrustedKeysPath(const std::string& path);

  const std::string& privateKeyPassword() const;
  void setPrivateKeyPassword(const std::string& password);

  void serialize(CryptoNote::ISerializer& s);
  void emplaceOptions(cxxopts::Options& options, Usage usage);
  void initializeServerContext(boost::asio::ssl::context& ctx);
  void initializeClientContext(boost::asio::ssl::context& ctx);

  /*!
   * \brief isInsecure checks for insecure setups depending on the usage of the configuration
   * \param usage Scenarios this configuration will be used for
   * \return True if the configuration is considered insecure for the usecase, otherwise false
   */
  bool isInsecure(Usage usage) const;

 private:
  /*!
   * \brief rootedPath Transaforms relative paths to be relative to the given rootPath
   * \param path The path to transform
   * \return The provided path if absolute, or a path relative to rootPath (if given)
   */
  std::string rootedPath(const std::string& path) const;

 private:
  bool m_enabled;
  bool m_verifyPeers;
  std::string m_rootPath;
  std::string m_certificatePath;
  std::string m_privateKeyPath;
  std::string m_dhparamPath;
  std::string m_trustedKeysPath;
  std::string m_privateKeyPassword;
};
}  // namespace Http
}  // namespace Xi
