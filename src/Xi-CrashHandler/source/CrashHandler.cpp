/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

#include "Xi/CrashHandler.h"

#include <stdexcept>
#include <utility>
#include <functional>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <iostream>
#include <codecvt>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/predef/os.h>
#include <handler/exception_handler.h>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Version.h>
#include <Xi/Config/Network.h>
#include <Xi/Utils/String.h>
#include <Xi/Http/MultipartFormDataBuilder.h>
#include <Xi/Http/Client.h>
#include <Common/Util.h>

#include "Xi/CrashUploader.h"

struct Xi::_CrashHandler_Impl : std::enable_shared_from_this<_CrashHandler_Impl> {
  std::unique_ptr<google_breakpad::ExceptionHandler> handle;
  CrashHandlerConfig config;

#if BOOST_OS_WINDOWS
  std::map<std::wstring, std::wstring> reportInfo;
  google_breakpad::CustomClientInfo nfo;
#endif  // BOOST_OS_WINDOWS
};

namespace {
static void upload(const std::string& file, const std::string& app) {
  std::cout << "Uploading..." << std::endl;
  Xi::CrashUploader uploader{Xi::Config::Network::breakpadServer(), 80};
  auto result = uploader.upload(file, app);
  if (result) {
    std::cout << "Upload succeeded. Thanks for supporting the project." << std::endl;
  } else {
    std::cout << "Upload failed. Thanks for supporting the project though." << std::endl;
  }
}

#if BOOST_OS_WINDOWS
static std::wstring to_wstring(const std::string str) {
  std::wstring wstr;
  wstr.resize(str.length() * 2 + 2, L'\0');
  size_t length = 0;
  auto err = mbstowcs_s(&length, &wstr[0], wstr.size(), str.data(), str.size());
  wstr.resize(length);
  if (err != 0) throw std::runtime_error{std::string{"Unable to convert '"} + str + "' to wide string."};
  return wstr;
}

static std::string to_string(const std::wstring wstr) {
  using convert_type = std::codecvt_utf8<wchar_t>;
  std::wstring_convert<convert_type, wchar_t> converter;
  return converter.to_bytes(wstr);
}

static bool dumpCallback(const wchar_t* dump_path, const wchar_t* minidump_id, void* context, EXCEPTION_POINTERS*,
                         MDRawAssertionInfo*, bool succeeded) {
  std::string file;
  {
    std::wstring wfile;
    wfile += dump_path;
    wfile += minidump_id;
    wfile += L".dmp";
    file = to_string(wfile);
  }

  if (succeeded) {
    std::cout << "Dump file created: " << file << std::endl;
    Xi::_CrashHandler_Impl* impl = static_cast<Xi::_CrashHandler_Impl*>(context);
    if (impl->config.IsUploadEnabled) upload(file, impl->config.Application);
  } else
    std::cout << "Dump file creation failed.";
  return succeeded;
}
#endif  // BOOST_OS_WINDOWS

#if BOOST_OS_LINUX
static bool dumpCallback(const google_breakpad::MinidumpDescriptor& descriptor, void* context, bool succeeded) {
  if (succeeded) {
    std::cout << "Dump file created: " << descriptor.path() << std::endl;
    const auto impl = static_cast<Xi::_CrashHandler_Impl*>(context);
    if (impl->config.IsUploadEnabled) upload(descriptor.path(), impl->config.Application);
  } else {
    std::cout << "Dump file creation failed.";
  }
  return succeeded;
}
#endif  // BOOST_OS_LINUX
}  // namespace

Xi::CrashHandler::CrashHandler(const Xi::CrashHandlerConfig& config) {
  using namespace std::placeholders;

  if (!Tools::create_directories_if_necessary(config.OutputPath))
    throw std::runtime_error{"Unable to create directory for crash dumps."};
  m_impl = std::make_shared<_CrashHandler_Impl>();
  m_impl->config = config;

#if BOOST_OS_WINDOWS
  m_impl->reportInfo[L"ver"] = to_wstring(std::string{PROJECT_VERSION_LONG});
  m_impl->reportInfo[L"prod"] = to_wstring(config.Application);
  m_impl->handle = std::make_unique<google_breakpad::ExceptionHandler>(
      to_wstring(config.OutputPath), nullptr, dumpCallback, m_impl.get(),
      google_breakpad::ExceptionHandler::HANDLER_ALL, MiniDumpNormal, L"", &m_impl->nfo);
#endif  // BOOST_OS_WINDOWS

#if BOOST_OS_LINUX
  const google_breakpad::MinidumpDescriptor desc{config.OutputPath};
  m_impl->handle =
      std::make_unique<google_breakpad::ExceptionHandler>(desc, nullptr, dumpCallback, m_impl.get(), true, -1);
#endif  // BOOST_OS_LINUX
}
