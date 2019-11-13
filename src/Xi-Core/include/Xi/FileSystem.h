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

#include <cinttypes>
#include <string>

#include <Xi/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/filesystem.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Result.h>
#include <Xi/Exceptional.hpp>

namespace Xi {
namespace FileSystem {

XI_DECLARE_EXCEPTIONAL_CATEGORY(FileSystem)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InsufficientSpace, "your hard drive has insufficient space for the operation",
                                FileSystem)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidType, "request encountered a file type that were not expected", FileSystem)

Xi::Result<boost::filesystem::space_info> availableSpace(const std::string& fileDirectory);
Xi::Result<boost::tribool> isRotationalDrive(const std::string& path);
Xi::Result<void> ensureDirectoryExists(const std::string& fileDirectory);
Xi::Result<void> removeDircetoryIfExists(const std::string& fileDirectory);
Xi::Result<bool> exists(const std::string& path);
Xi::Result<void> removeFileIfExists(const std::string& path);

/*!
 * \brief searchFile Tries to find a file for a given path.
 * \param filepath The given path to search for
 * \param fileEnding If not empty tries to add the file ending that may not been provided.
 * \param dataDir A user provided data, the file may be relative to it.
 * \return The normal form path if found otherwise an error.
 */
Xi::Result<std::string> searchFile(const std::string& filepath, const std::string& fileEnding = "",
                                   const std::string& dataDir = "");

Xi::Result<std::string> fileDirectory(const std::string& path);

}  // namespace FileSystem
}  // namespace Xi
