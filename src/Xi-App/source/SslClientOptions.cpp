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

#include "Xi/App/SslClientOptions.h"

#include <Xi/Exceptions.hpp>
#include <Xi/FileSystem.h>

namespace {
XI_DECLARE_EXCEPTIONAL_CATEGORY(SslClientOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(MissingFile, "required ssl client file is missing", SslClientOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidFile, "required ssl client file is invalid", SslClientOptions)
}  // namespace

namespace Xi {
namespace App {

SslClientOptions::SslClientOptions(const SslOptions &base) : m_baseOptions(base) {
  /* */
}

void SslClientOptions::loadEnvironment(Environment &env) {
  // clang-format off
  env
    (trusted(), "SSL_TRUSTED")
    (noVerify(), "SSL_NO_VERIFY")
  ;
  // clang-format on
}

void SslClientOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
    options.add_options("ssl")
        ("ssl-trusted", "Filepath containing server certficates you want to trust.",
            cxxopts::value<std::string>(trusted())->default_value("trusted.pem"))

        ("ssl-no-verify", "Disables peer verification.",
            cxxopts::value<bool>(noVerify())->default_value(noVerify() ? "true" : "false")->implicit_value("true"));
  // clang-format on
}

bool SslClientOptions::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  XI_UNUSED(options, result);
  XI_RETURN_SC_IF(noVerify(), false);
  XI_RETURN_SC_IF_NOT(m_baseOptions.enabled(), false);

  exceptional_if<MissingFileError>(FileSystem::searchFile(trusted(), ".pem", m_baseOptions.rootDir()).isError());

  XI_RETURN_SC(false);
}

void SslClientOptions::configure(Http::SSLConfiguration &config) const {
  config.setVerifyPeers(!noVerify());
  if (!m_baseOptions.enabled() || noVerify()) {
    return;
  }
  config.setTrustedKeysPath(FileSystem::searchFile(trusted(), ".pem", m_baseOptions.rootDir()).takeOrThrow());
}

}  // namespace App
}  // namespace Xi
