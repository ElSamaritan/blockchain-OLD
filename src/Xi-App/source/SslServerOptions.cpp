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

#include "Xi/App/SslServerOptions.h"

#include <Xi/ExternalIncludePush.h>
#include <boost/asio/ssl.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Exceptional.hpp>
#include <Xi/FileSystem.h>

namespace {
XI_DECLARE_EXCEPTIONAL_CATEGORY(SslServerOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(MissingFile, "required ssl server file is missing", SslServerOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidFile, "required ssl server file is invalid", SslServerOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidPassword, "private key password is incorrect", SslServerOptions)
}  // namespace

namespace Xi {
namespace App {

SslServerOptions::SslServerOptions(const SslOptions &base) : m_baseOptions{base} {
}

void SslServerOptions::loadEnvironment(Environment &env) {
  // clang-format off
  env
    (certificate(), "SSL_CERT")
    (privateKey(), "SSL_PRIVATE_KEY")
    (privateKeyPassword(), "SSL_PRIVATE_KEY_PASSWORD")
    (diffieHellman(), "SSL_DH_PARAM")
  ;
  // clang-format on
}

void SslServerOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
    options.add_options("ssl")
        ("ssl-cert", "Filepath containing the server certificate to use.",
            cxxopts::value<std::string>(certificate())->default_value(certificate()))

        ("ssl-pk", "Filepath containing the server private key for encryption.",
            cxxopts::value<std::string>(privateKey())->default_value(privateKey()))

        ("ssl-pkp", "Password of the private key file used by the server.",
            cxxopts::value<std::string>(privateKeyPassword())->default_value(privateKeyPassword()))

        ("ssl-dh", "Filepath containing the dhparam randomization for the server.",
            cxxopts::value<std::string>(diffieHellman())->default_value(diffieHellman()));
  // clang-format on
}

bool SslServerOptions::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  XI_UNUSED(options, result);
  XI_RETURN_SC_IF_NOT(m_baseOptions.enabled(), false);

  exceptional_if<MissingFileError>(FileSystem::searchFile(certificate(), ".pem", m_baseOptions.rootDir()).isError());
  exceptional_if<MissingFileError>(FileSystem::searchFile(privateKey(), ".pem", m_baseOptions.rootDir()).isError());
  exceptional_if<MissingFileError>(FileSystem::searchFile(diffieHellman(), ".pem", m_baseOptions.rootDir()).isError());

  XI_RETURN_SC(false);
}

void SslServerOptions::configure(Http::SSLConfiguration &config) const {
  if (!m_baseOptions.enabled()) {
    return;
  }
  config.setCertificatePath(FileSystem::searchFile(certificate(), ".pem", m_baseOptions.rootDir()).takeOrThrow());
  config.setPrivateKeyPath(FileSystem::searchFile(privateKey(), ".pem", m_baseOptions.rootDir()).takeOrThrow());
  config.setPrivateKeyPassword(privateKeyPassword());
  config.setDhparamPath(FileSystem::searchFile(diffieHellman(), ".pem", m_baseOptions.rootDir()).takeOrThrow());
}

}  // namespace App
}  // namespace Xi
