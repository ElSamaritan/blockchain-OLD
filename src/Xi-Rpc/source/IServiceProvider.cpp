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

#include "Xi/Rpc/IServiceProvider.hpp"

#include <Xi/Global.hh>
#include <Xi/Exceptions.hpp>

namespace Xi {
namespace Rpc {

IServiceProvider::IServiceProvider(Logging::ILogger& logger) : m_logger(logger, "Service") { /* */
}

ServiceError IServiceProvider::operator()(std::string_view command, CryptoNote::ISerializer& input,
                                          CryptoNote::ISerializer& output) {
  try {
    return process(command, input, output);
  }

  catch (IndexOutOfRangeError& e) {
    logThrow(e);
    return ServiceError::InvalidArgument;
  } catch (OutOfRangeError& e) {
    logThrow(e);
    return ServiceError::InvalidArgument;
  } catch (InvalidIndexError& e) {
    logThrow(e);
    return ServiceError::InvalidArgument;
  } catch (InvalidSizeError& e) {
    logThrow(e);
    return ServiceError::InvalidArgument;
  } catch (InvalidVariantTypeError& e) {
    logThrow(e);
    return ServiceError::InvalidArgument;
  } catch (FormatError& e) {
    logThrow(e);
    return ServiceError::InvalidArgument;
  } catch (NotImplementedError& e) {
    logThrow(e);
    return ServiceError::NotInitialized;
  } catch (NotFoundError& e) {
    logThrow(e);
    return ServiceError::Exceptional;
  } catch (InvalidArgumentError& e) {
    logThrow(e);
    return ServiceError::InvalidArgument;
  } catch (NotInitializedError& e) {
    logThrow(e);
    return ServiceError::NotInitialized;
  } catch (std::exception& e) {
    logThrow(e);
    return ServiceError::NotInitialized;
  } catch (...) {
    m_logger(Logging::Fatal) << "Service operator threw: UNKNOWN EXCEPTION TYPE";
    return ServiceError::Exceptional;
  }
}

void IServiceProvider::logThrow(std::exception& e) {
  m_logger(Logging::Error) << "Service operator threw: " << e.what();
}

}  // namespace Rpc
}  // namespace Xi
