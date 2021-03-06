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

#pragma once

#include <Xi/Global.hh>
#include <Xi/Http/SSLConfiguration.h>
#include <Serialization/ISerializer.h>

#include "Xi/App/IOptions.h"
#include "Xi/App/SslOptions.h"

namespace Xi {
namespace App {

class SslClientOptions : public IOptions {
 public:
  SslClientOptions(const SslOptions& base);
  ~SslClientOptions() override = default;

 public:
  XI_PROPERTY(bool, noVerify, false)
  XI_PROPERTY(std::string, trusted, "./trusted.pem")

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(noVerify(), no_verfiy)
  KV_MEMBER_RENAME(trusted(), trusted)
  KV_END_SERIALIZATION

  void loadEnvironment(Environment& env) override;
  void emplaceOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;

  void configure(Http::SSLConfiguration& config) const;

 private:
  const SslOptions& m_baseOptions;
};

}  // namespace App
}  // namespace Xi
