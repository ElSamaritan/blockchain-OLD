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

#include "Xi/Http/SSLServerConfiguration.h"

#include <string>
#include <fstream>
#include <streambuf>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/filesystem.hpp>
#include <boost/asio/ssl.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

namespace {
const std::string CLIPrefix{"ssl-server-"};
const std::string DisabledName{"disable"};
const std::string CertFileName{"cert"};
const std::string KeyFileName{"key"};
const std::string PasswordName{"key-password"};
const std::string DhFileName{"dhparam"};
}  // namespace

Xi::Http::SSLServerConfiguration::SSLServerConfiguration() {
  Disabled = false;
  CertFile = "./certs/cert.pem";
  KeyFile = "./certs/key.pem";
  Password = "";
  DhFile = "./certs/dh.pem";
}

void Xi::Http::SSLServerConfiguration::initializeContext(boost::asio::ssl::context& ctx) {
  using namespace boost::filesystem;
  if (Disabled) return;
  if (!exists(CertFile)) throw std::runtime_error{"CertFile could not be found."};
  if (!exists(KeyFile)) throw std::runtime_error{"KeyFile could not be found."};
  if (!exists(DhFile)) throw std::runtime_error{"DhFile could not be found."};

  ctx.set_password_callback(
      [Password = this->Password](std::size_t, boost::asio::ssl::context_base::password_purpose) { return Password; });

  ctx.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 |
                  boost::asio::ssl::context::single_dh_use);

  boost::system::error_code ec;
  ctx.use_certificate_chain_file(CertFile, ec);
  if (ec) throw std::runtime_error{"unable to load certificate"};
  ctx.use_private_key_file(KeyFile, boost::asio::ssl::context::file_format::pem, ec);
  if (ec) throw std::runtime_error{"unable to load private key"};
  ctx.use_tmp_dh_file(DhFile, ec);
  if (ec) throw std::runtime_error{"unable to load dhparam file"};
}

void Xi::Http::SSLServerConfiguration::emplaceCLIConfiguration(cxxopts::Options& opts) {
  // clang-format off
  opts.add_options("SSL-Server")
      (CLIPrefix + DisabledName, "Disables the ssl capabilities of the server. This is very unsafe and can result in "
                      "Man in the Middle attacks and expose private information.",
       cxxopts::value<bool>(Disabled)->implicit_value("true")->default_value("false"))

      (CLIPrefix + CertFileName, "Filename of the public key to use.", cxxopts::value<std::string>(CertFile)->default_value("./certs/cert.pem"))
      (CLIPrefix + KeyFileName, "Filename of the private key to use.", cxxopts::value<std::string>(KeyFile)->default_value("./certs/key.pem"))
      (CLIPrefix + DhFileName, "Filename of the dh param to use.", cxxopts::value<std::string>(DhFile)->default_value("./certs/dh.pem"))
      (CLIPrefix + PasswordName, "Password to access the private key.", cxxopts::value<std::string>(Password)->default_value(""));
  // clang-format on
}

void Xi::Http::SSLServerConfiguration::load(const nlohmann::json& json) {
  if (json.find(DisabledName) != json.end()) {
    Disabled = json[DisabledName].get<bool>();
  }
  if (json.find(CertFileName) != json.end()) {
    CertFile = json[CertFileName].get<std::string>();
  }
  if (json.find(KeyFileName) != json.end()) {
    KeyFile = json[KeyFileName].get<std::string>();
  }
  if (json.find(DhFileName) != json.end()) {
    DhFile = json[DhFileName].get<std::string>();
  }
  if (json.find(PasswordName) != json.end()) {
    Password = json[PasswordName].get<std::string>();
  }
}

void Xi::Http::SSLServerConfiguration::store(nlohmann::json& json) const {
  json[DisabledName] = Disabled;
  json[CertFileName] = CertFile;
  json[KeyFileName] = KeyFile;
  json[DhFileName] = DhFile;
  json[PasswordName] = Password;
}

bool Xi::Http::SSLServerConfiguration::isInsecure() const { return Disabled || Password.size() < 10; }
