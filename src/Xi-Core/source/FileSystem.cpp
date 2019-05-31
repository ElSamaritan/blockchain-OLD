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

#include "Xi/FileSystem.h"

Xi::Result<boost::filesystem::space_info> Xi::FileSystem::availableSpace(const std::string &directory) {
  XI_ERROR_TRY();
  boost::filesystem::path path(directory);
  return success(boost::filesystem::space(path));
  XI_ERROR_CATCH();
}

Xi::Result<boost::logic::tribool> Xi::FileSystem::isRotationalDrive(const std::string &path) {
  XI_UNUSED(path);
  XI_ERROR_TRY();
  return success<boost::logic::tribool>(boost::logic::indeterminate);
  XI_ERROR_CATCH();
}

Xi::Result<void> Xi::FileSystem::ensureDirectoryExists(const std::string &directory) {
  using namespace boost::filesystem;
  boost::system::error_code ec;
  boost::filesystem::path path{directory};
  if (boost::filesystem::is_directory(path, ec)) {
    return success();
  }
  if (boost::filesystem::create_directories(path, ec)) {
    return success();
  } else {
    return makeError(ec);
  }
}

Xi::Result<void> Xi::FileSystem::removeDircetoryIfExists(const std::string &directory) {
  XI_ERROR_TRY();
  using namespace boost::filesystem;
  boost::filesystem::path path{directory};
  if (!exists(path)) {
    return success();
  }
  boost::system::error_code ec;
  if (boost::filesystem::is_directory(path, ec)) {
    remove_all(path, ec);
    if (ec) {
      return failure(ec);
    } else {
      return success();
    }
  } else {
    return failure(ec);
  }
  XI_ERROR_CATCH();
}

Xi::Result<bool> Xi::FileSystem::exists(const std::string &path) { return success(boost::filesystem::exists(path)); }

Xi::Result<void> Xi::FileSystem::removeFileIfExists(const std::string &p) {
  XI_ERROR_TRY();
  boost::system::error_code ec;
  if (exists(p).takeOrThrow()) {
    if (boost::filesystem::is_regular_file(p, ec)) {
      boost::filesystem::remove(p, ec);
      if (ec) {
        return makeError(ec);
      }
    } else {
      exceptional<InvalidTypeError>("file removal was request but the path not points to a regular file");
    }

    if (ec) {
      return makeError(ec);
    }
  }
  return success();
  XI_ERROR_CATCH();
}
