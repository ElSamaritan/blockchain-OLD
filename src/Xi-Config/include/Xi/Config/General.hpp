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

#include <string>

#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>

namespace Xi {
namespace Config {

namespace General {

class Configuration {
 public:
  XI_PROPERTY(std::string, homepage)
  XI_PROPERTY(std::string, description)
  XI_PROPERTY(std::string, copyright)
  XI_PROPERTY(std::string, contactUrl)
  XI_PROPERTY(std::string, licenseUrl)
  XI_PROPERTY(std::string, downloadUrl)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(homepage(), homepage)
  KV_MEMBER_RENAME(description(), description)
  KV_MEMBER_RENAME(copyright(), copyright)
  KV_MEMBER_RENAME(contactUrl(), contact_url)
  KV_MEMBER_RENAME(licenseUrl(), license_url)
  KV_MEMBER_RENAME(downloadUrl(), download_url)
  KV_END_SERIALIZATION
};

}  // namespace General
}  // namespace Config
}  // namespace Xi
