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

#include <Xi/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <Xi/ExternalIncludePop.h>

#include <CryptoNoteCore/Currency.h>
#include <Serialization/ISerializer.h>

#include <string>

namespace CommonCLI {

struct VersionInformation {
  std::string version;
  std::string copyright;

  struct Build {
    std::string channel;
    std::string type;
    bool dev;

    struct Compiler {
      std::string cc;
      std::string cxx;

      KV_BEGIN_SERIALIZATION
      KV_MEMBER(cc)
      KV_MEMBER(cxx)
      KV_END_SERIALIZATION
    } compiler;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(channel)
    KV_MEMBER(type)
    KV_MEMBER(dev)
    KV_MEMBER(compiler)
    KV_END_SERIALIZATION

  } build;

  struct Git {
    std::string commit;
    std::string branch;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(commit)
    KV_MEMBER(branch)
    KV_END_SERIALIZATION
  } git;

  struct External {
    std::string boost;
    std::string openssl;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(boost)
    KV_MEMBER(openssl)
    KV_END_SERIALIZATION
  } external;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(version)
  KV_MEMBER(build)
  KV_MEMBER(git)
  KV_MEMBER(external)
  KV_MEMBER(copyright)
  KV_END_SERIALIZATION
};

/*!
 * \brief header returns an appropiate header to display including a message telling you you are
 * on a testing version, if so
 * \param colored If true emplaces color encoding used by the logging library
 * \return a header to display at startup
 */
std::string header(const CryptoNote::Currency& currency, bool colored = false);

/*!
 * \brief checks wheter this version was built from non master branch source code
 * \return true if this version is not built from master branch, otherwise false
 */
bool isDevVersion();

/*!
 * \brief devWarning returns a message to be printed if a dev version is running.
 */
std::string devWarning();

/*!
 * \brief insecureClientWarning returns a message to be printed for insecure client setups
 */
std::string insecureClientWarning();

/*!
 * \brief insecureClientWarning returns a message to be printed for insecure server setups
 */
std::string insecureServerWarning();

/*!
 * \brief verboseVersionInformation returns a verbose version information string about this build.
 */
std::string verboseVersionInformation();

/*!
 * \brief versionInformation returns structured verbose version information.
 */
VersionInformation versionInformation();

/*!
 * \brief emplaceCLIOptions will add common options for CLI applications to the option parser interface
 * \param options The parser that will handle the options
 */
void emplaceCLIOptions(cxxopts::Options& options);

/*!
 * \brief handleCLIOptions handles common options given by the CLI
 * \param option The options that have been parsed
 * \param result The parsed options result
 * \return true if the application should exit, otherwise false
 */
bool handleCLIOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result);

/*!
 * \brief make_crash_dumper creates a crash dumper if breakpad was linked and enabled.
 * \param the application running the crash dumper, used to determine in which application the bug occured
 * \return a null pointer the actual crash dumper implementation.
 */
void* make_crash_dumper(const std::string& applicationId);
}  // namespace CommonCLI
