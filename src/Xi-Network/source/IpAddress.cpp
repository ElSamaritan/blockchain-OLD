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

#include "Xi/Network/IpAddress.hpp"

#include <cstring>
#include <utility>
#include <algorithm>

#include <Xi/ExternalIncludePush.h>
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Exceptions.hpp>

namespace Xi {
namespace Network {

Result<IpAddress> IpAddress::fromString(const std::string& str) {
  XI_ERROR_TRY
  IpAddress reval{};
  boost::system::error_code ec{};
  const auto ip = boost::asio::ip::address::from_string(str, ec);
  exceptional_if<RuntimeError>(ec.failed(), ec.message());
  if (ip.is_v4()) {
    const auto ip4 = ip.to_v4().to_bytes();
    v4_storage storage{};
    std::memcpy(storage.data(), ip4.data(), ip4.size());
    reval.m_data = storage;
  } else if (ip.is_v6()) {
    const auto ip6 = ip.to_v6().to_bytes();
    v6_storage storage{};
    std::memcpy(storage.data(), ip6.data(), ip6.size());
    reval.m_data = storage;
  } else {
    exceptional<InvalidVariantTypeError>();
  }
  return success(reval);
  XI_ERROR_CATCH
}

Result<IpAddressVector> IpAddress::resolve(const std::string& host) {
  return resolve(host, Port::Any);
}

Result<IpAddressVector> IpAddress::resolve(const std::string& host, const Port port) {
  XI_ERROR_TRY
  if (const auto plain = fromString(host); plain.isValue()) {
    return success(IpAddressVector{{plain.value()}});
  }
  boost::asio::io_context io{};
  boost::asio::ip::tcp::resolver::query query{host, port.toString()};
  boost::asio::ip::tcp::resolver resolver{io};
  boost::system::error_code ec{};
  auto begin = resolver.resolve(query, ec);
  exceptional_if<RuntimeError>(ec.failed(), "Host resolution for '{}' failed: {}", host, ec.message());
  boost::asio::ip::tcp::resolver::iterator end{};

  IpAddressVector reval{};
  for (; begin != end; ++begin) {
    IpAddress address{};
    const auto ip = begin->endpoint().address();
    if (ip.is_v4()) {
      const auto ip4 = ip.to_v4().to_bytes();
      v4_storage storage{};
      std::memcpy(storage.data(), ip4.data(), ip4.size());
      address.m_data = storage;
    } else if (ip.is_v6()) {
      const auto ip6 = ip.to_v6().to_bytes();
      v6_storage storage{};
      std::memcpy(storage.data(), ip6.data(), ip6.size());
      address.m_data = storage;
    } else {
      exceptional<InvalidVariantTypeError>();
    }
    reval.emplace_back(std::move(address));
  }
  return success(reval);
  XI_ERROR_CATCH
}

Result<IpAddress> IpAddress::resolveAny(const std::string& host) {
  XI_ERROR_TRY
  const auto addresses = resolve(host).takeOrThrow();
  exceptional_if<NotFoundError>(addresses.empty());
  return success(addresses.front());
  XI_ERROR_CATCH
}

Result<IpAddress> IpAddress::resolveAny(const std::string& host, const IpAddress::Type type) {
  XI_ERROR_TRY
  const auto addresses = resolve(host).takeOrThrow();
  const auto search =
      std::find_if(addresses.begin(), addresses.end(), [type](const auto& i) { return i.type() == type; });
  exceptional_if<NotFoundError>(search == addresses.end());
  return success(*search);
  XI_ERROR_CATCH
}

IpAddress::Type IpAddress::type() const {
  if (std::holds_alternative<v4_storage>(m_data)) {
    return v4;
  } else if (std::holds_alternative<v6_storage>(m_data)) {
    return v6;
  } else {
    exceptional<InvalidVariantTypeError>();
  }
}

Result<uint32_t> IpAddress::v4Address() const {
  if (std::holds_alternative<v4_storage>(m_data)) {
    const auto& storage = std::get<v4_storage>(m_data);
    const auto reval = *reinterpret_cast<const uint32_t*>(storage.data());
    return success(static_cast<uint32_t>(reval));
  } else {
    exceptional<InvalidVariantTypeError>();
  }
}

const Byte* IpAddress::data() const {
  if (std::holds_alternative<v4_storage>(m_data)) {
    return std::get<v4_storage>(m_data).data();
  } else if (std::holds_alternative<v6_storage>(m_data)) {
    return std::get<v6_storage>(m_data).data();
  } else {
    exceptional<InvalidVariantTypeError>();
  }
}

size_t IpAddress::size() const {
  if (std::holds_alternative<v4_storage>(m_data)) {
    return 4;
  } else if (std::holds_alternative<v6_storage>(m_data)) {
    return 16;
  } else {
    exceptional<InvalidVariantTypeError>();
  }
}

std::string IpAddress::toString() const {
  if (std::holds_alternative<v4_storage>(m_data)) {
    const auto& storage = std::get<v4_storage>(m_data);
    return boost::asio::ip::make_address_v4(storage).to_string();
  } else if (std::holds_alternative<v6_storage>(m_data)) {
    const auto& storage = std::get<v6_storage>(m_data);
    return boost::asio::ip::make_address_v6(storage).to_string();
  } else {
    exceptional<InvalidVariantTypeError>();
  }
}

IpAddress::IpAddress() {
  /* */
}

std::string toString(const IpAddress& addr) {
  return addr.toString();
}

}  // namespace Network
}  // namespace Xi
