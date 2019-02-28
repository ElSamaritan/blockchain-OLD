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

#pragma once

#include <cinttypes>
#include <string>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/filesystem.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Result.h>
#include <Xi/Exceptional.h>

namespace Xi {
namespace FileSystem {

XI_DECLARE_EXCEPTIONAL_CATEGORY(FileSystem)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InsufficientSpace, "your hard drive has insufficient space for the operation",
                                FileSystem)

Xi::Result<boost::filesystem::space_info> availableSpace(const std::string& directory);
Xi::Result<boost::tribool> isRotationalDrive(const std::string& path);
Xi::Result<void> ensureDirectoryExists(const std::string& directory);
Xi::Result<void> removeDircetoryIfExists(const std::string& directory);
Xi::Result<bool> exists(const std::string& path);

}  // namespace FileSystem
}  // namespace Xi
