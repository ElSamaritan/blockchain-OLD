﻿/* ============================================================================================== *
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
#include <boost/optional.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Global.h>

namespace Xi {
/*!
 * \brief The CrashUploader class encapsulates the process of uploading a crash dump to the breakpad server
 */
class CrashUploader {
 public:
  /*!
   * \brief CrashUploader constructs a new object to upload crash dumps
   * \param breapkpadHost The server hosting the breakpad server
   * \param port The server post for reaching the breakpad server
   */
  explicit CrashUploader(const std::string &breapkpadHost, uint16_t port);
  XI_DELETE_COPY(CrashUploader);
  XI_DEFAULT_MOVE(CrashUploader);
  ~CrashUploader();

  /*!
   * \brief upload sends a crash dump to the breakpad server
   * \param file The crash dump file path
   * \param application The application that produced the crash dump
   * \return A string if the uploaded succeeded, otherwise no value
   */
  boost::optional<std::string> upload(const std::string &file, const std::string &application);

 private:
  const std::string m_host;
  const uint16_t m_port;
};
}  // namespace Xi