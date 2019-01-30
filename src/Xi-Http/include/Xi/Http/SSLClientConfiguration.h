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
  static const SSLClientConfiguration NoSSL;

  bool Disabled;
  bool VerifyPeers;
  std::string TrustedFile;

  explicit SSLClientConfiguration(bool disabled = false, bool verifiyPeers = true,
                                  const std::string& trustedFile = "./certs/trusted.pem");
  ~SSLClientConfiguration() = default;

  void initializeContext(boost::asio::ssl::context& ctx);

  void emplaceCLIConfiguration(cxxopts::Options& opts);

  void load(const nlohmann::json& json);
  void store(nlohmann::json& json) const;

  bool isInsecure() const;
};
}  // namespace Http
}  // namespace Xi
