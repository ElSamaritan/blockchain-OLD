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

#include "Xi/CrashUploader.h"

#include <Xi/Version/Version.h>
#include <Xi/Config/Network.h>
#include <Xi/Http/Client.h>
#include <Xi/Http/MultipartFormDataBuilder.h>

Xi::CrashUploader::CrashUploader(const std::string& breapkpadHost, uint16_t port)
    : m_host{breapkpadHost}, m_port{port} {
}

Xi::CrashUploader::~CrashUploader() {
}

boost::optional<std::string> Xi::CrashUploader::upload(const std::string& file, const std::string& application) {
  try {
    Xi::Http::Client client{std::string{"https://"} + m_host + ":" + std::to_string(m_port),
                            Xi::Http::SSLConfiguration{}};
    Xi::Http::MultipartFormDataBuilder builder;
    builder.addField("prod", application);
    builder.addField("ver", APP_VERSION);
    builder.addFile("upload_file_minidump", file);
    auto request = builder.request("/crashreports");
    const auto response = client.send(std::move(request)).get();
    return response.body();
  } catch (...) {
    return boost::optional<std::string>{};
  }
}
