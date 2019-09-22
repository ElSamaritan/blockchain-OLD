﻿/* ============================================================================================== *
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

#include "Xi/FileSystem.h"

#include <fstream>

#include <Xi/Exceptions.hpp>

Xi::Result<boost::filesystem::space_info> Xi::FileSystem::availableSpace(const std::string &directory) {
  XI_ERROR_TRY
  boost::filesystem::path path(directory);
  return success(boost::filesystem::space(path));
  XI_ERROR_CATCH
}

Xi::Result<boost::logic::tribool> Xi::FileSystem::isRotationalDrive(const std::string &path) {
  XI_UNUSED(path);
  XI_ERROR_TRY
  return success<boost::logic::tribool>(boost::logic::indeterminate);
  XI_ERROR_CATCH
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
  XI_ERROR_TRY
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
  XI_ERROR_CATCH
}

Xi::Result<bool> Xi::FileSystem::exists(const std::string &path) {
  return success(boost::filesystem::exists(path));
}

Xi::Result<void> Xi::FileSystem::removeFileIfExists(const std::string &p) {
  XI_ERROR_TRY
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
  XI_ERROR_CATCH
}

Xi::Result<std::string> Xi::FileSystem::searchFile(const std::string &filepath, const std::string &fileEnding,
                                                   const std::string &dataDir) {
  using namespace boost::filesystem;
  XI_ERROR_TRY
  std::vector<path> alternatives{{path{filepath}}};
  if (!fileEnding.empty()) {
    alternatives.push_back(path{filepath + fileEnding});
  }
  if (!dataDir.empty()) {
    path dataPath{dataDir};
    alternatives.push_back(dataPath / filepath);
    if (!fileEnding.empty()) {
      alternatives.push_back(dataPath / (filepath + fileEnding));
    }
  }
  for (const auto &alternative : alternatives) {
    boost::system::error_code ec{};
    auto canoncialAlternative = canonical(alternative, ec);
    if (ec) {
      continue;
    }
    if (!boost::filesystem::exists(canoncialAlternative, ec) || ec) {
      continue;
    }
    if (!is_regular_file(canoncialAlternative, ec) || ec) {
      continue;
    }
    return success(canoncialAlternative.string());
  }
  exceptional<NotFoundError>("file not found: {0} (file ending: {1}, data directory: {2})", filepath, fileEnding,
                             dataDir);
  XI_ERROR_CATCH
}

Xi::Result<std::string> Xi::FileSystem::readTextFile(const std::string &path) {
  XI_ERROR_TRY
  std::ifstream stream(path);
  return emplaceSuccess<std::string>(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
  XI_ERROR_CATCH
}

Xi::Result<Xi::ByteVector> Xi::FileSystem::readBinaryFile(const std::string &path) {
  XI_ERROR_TRY
  std::ifstream stream(path, std::ios::binary);
  std::string str{std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
  ByteVector reval{};
  reval.resize(str.size());
  std::memcpy(reval.data(), str.data(), reval.size());
  return success(std::move(reval));
  XI_ERROR_CATCH
}

Xi::Result<std::string> Xi::FileSystem::directory(const std::string &path) {
  XI_ERROR_TRY
  boost::filesystem::path bpath{path};
  boost::system::error_code ec{};
  bpath = boost::filesystem::canonical(bpath, ec);
  if (ec) {
    return makeError(ec);
  }

  if (boost::filesystem::is_directory(bpath)) {
    return success(bpath.string());
  } else {
    return success(bpath.parent_path().string());
  }

  XI_ERROR_CATCH
}

Xi::Result<std::string> Xi::FileSystem::combine(const std::string &path, const std::string &subpath) {
  XI_ERROR_TRY
  boost::filesystem::path bpath = boost::filesystem::path{path} / boost::filesystem::path{subpath};
  return success(bpath.string());
  XI_ERROR_CATCH
}

Xi::Result<std::string> Xi::FileSystem::rooted(const std::string &path, const std::string &root) {
  XI_ERROR_TRY
  boost::filesystem::path bpath{path};
  boost::system::error_code ec{};

  if (bpath.is_relative()) {
    if (!root.empty()) {
      bpath = boost::filesystem::path{root} / path;
    }
    bpath = boost::filesystem::canonical(bpath, ec);
    if (ec) {
      return makeError(ec);
    }
    return success(bpath.string());
  } else {
    return success(bpath.string());
  }

  XI_ERROR_CATCH
}

Xi::Result<void> Xi::FileSystem::writeTextFile(const std::string &path, const std::string &content, bool allowTrunc) {
  XI_ERROR_TRY
  exceptional_if<AlreadyExistsError>(exists(path).valueOrThrow() && !allowTrunc);
  std::ofstream stream{path, std::ios::out | std::ios::trunc};
  stream << content;
  return success();
  XI_ERROR_CATCH
}

Xi::Result<void> Xi::FileSystem::writeBinaryFile(const std::string &path, Xi::ConstByteSpan content, bool allowTrunc) {
  XI_ERROR_TRY
  exceptional_if<AlreadyExistsError>(exists(path).valueOrThrow() && !allowTrunc);
  std::ofstream stream{path, std::ios::out | std::ios::trunc | std::ios::binary};
  stream.write(reinterpret_cast<const char *>(content.data()), static_cast<std::streamsize>(content.size()));
  return success();
  XI_ERROR_CATCH
}
