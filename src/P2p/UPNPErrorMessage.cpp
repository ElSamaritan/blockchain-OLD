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

#include "P2p/UPNPErrorMessage.h"

#include <miniupnpc/upnpcommands.h>

std::string CryptoNote::get_upnp_error_message(int codec) {
  switch (codec) {
    case UPNPCOMMAND_SUCCESS:
      return "success";
    case UPNPCOMMAND_UNKNOWN_ERROR:
      return "unknown";
    case UPNPCOMMAND_INVALID_ARGS:
      return "invalid arguments passed";
    case UPNPCOMMAND_HTTP_ERROR:
      return "invalid http response";
    case UPNPCOMMAND_INVALID_RESPONSE:
      return "invalid peer response";
    case UPNPCOMMAND_MEM_ALLOC_ERROR:
      return "memory allocation failed";
    case 402:
      return "invalid arguments passed";
    case 501:
      return "action failed";
    case 715:
      return "the source ip cannot be wild-carded";
    case 716:
      return "the external port cannot be wild-carded";
    case 718:
      return "internal and external port values must be the same";
    case 725:
      return "the nat implementation only supports permanent lease times on port mappings";
    case 726:
      return "RemoteHost must be a wildcard and cannot be a specific IP address or DNS name";
    case 727:
      return "ExternalPort must be a wildcard and cannot be a specific port value";
    case 728:
      return "There are not enough free ports available to complete port mapping.";
    case 729:
      return "Attempted port mapping is not allowed due to conflict with other mechanisms.";
    case 732:
      return "The internal port cannot be wild-carded";
    default:
      return "unknown";
  }
  return "unknown";
}
