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

#include "Xi/Utils/FileSystem.h"

Xi::Result<boost::filesystem::space_info> Xi::FileSystem::availableSpace(const std::string &directory) {
  XI_ERROR_TRY();
  boost::filesystem::path path(directory);
  return boost::filesystem::space(path);
  XI_ERROR_CATCH();
}

Xi::Result<boost::logic::tribool> Xi::FileSystem::isRotationalDrive(const std::string &path) {
  XI_UNUSED(path);
  XI_ERROR_TRY();
  return make_result<boost::logic::tribool>(boost::logic::indeterminate);
  XI_ERROR_CATCH();
}

Xi::Result<void> Xi::FileSystem::ensureDirectoryExists(const std::string &directory) {
  using namespace boost::filesystem;
  boost::system::error_code ec;
  boost::filesystem::path path{directory};
  if (boost::filesystem::is_directory(path, ec)) {
    return make_result<void>();
  }
  if (boost::filesystem::create_directories(path, ec)) {
    return make_result<void>();
  } else {
    return make_error(ec);
  }
}

Xi::Result<void> Xi::FileSystem::removeDircetoryIfExists(const std::string &directory) {
  XI_ERROR_TRY();
  using namespace boost::filesystem;
  boost::filesystem::path path{directory};
  if (!exists(path)) {
    return make_result<void>();
  }
  boost::system::error_code ec;
  if (boost::filesystem::is_directory(path, ec)) {
    remove_all(path, ec);
    if (ec) {
      return make_error(ec);
    } else {
      return make_result<void>();
    }
  } else {
    return make_error(ec);
  }
  XI_ERROR_CATCH();
}
