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

#include "CryptoNoteCore/AmountFormatter.hpp"

#include <sstream>
#include <iomanip>
#include <limits>

#include <Xi/Algorithm/Math.h>
#include <Xi/Exceptions.hpp>

namespace CryptoNote {

AmountFormatter::AmountFormatter(const uint64_t _decimals, std::string_view _ticker, const std::locale& _locale)
    : m_decimals{_decimals}, m_ticker{_ticker}, m_locale{_locale} {
  Xi::exceptional_if<Xi::OutOfRangeError>(_decimals == 0 || _decimals > std::numeric_limits<uint64_t>::digits10);
}

AmountFormatter::~AmountFormatter() {}

std::string AmountFormatter::operator()(const uint64_t amount, bool addTicker) const {
  const auto divisor = Xi::pow64(10, decimals());
  const uint64_t major = amount / divisor;
  const uint64_t minor = amount % divisor;

  const auto& numpunct = std::use_facet<std::numpunct<char>>(m_locale);

  std::stringstream builder{};
  builder << major << numpunct.decimal_point() << std::setw(static_cast<int>(decimals())) << std::setfill('0') << minor;
  if (addTicker) {
    builder << " " << m_ticker;
  }
  return builder.str();
}

std::string AmountFormatter::operator()(const int64_t amount, bool addTicker) const {
  if (amount < 0) {
    return std::string{"-"} + (*this)(static_cast<uint64_t>(amount), addTicker);
  } else {
    return (*this)(static_cast<uint64_t>(amount), addTicker);
  }
}

uint64_t AmountFormatter::decimals() const { return m_decimals; }

const std::string& AmountFormatter::ticker() const { return m_ticker; }

const std::locale& AmountFormatter::locale() const { return m_locale; }

}  // namespace CryptoNote
