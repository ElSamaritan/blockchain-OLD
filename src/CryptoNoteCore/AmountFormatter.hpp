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
#include <string_view>
#include <string>
#include <locale>

#include "CryptoNoteCore/IAmountFormatter.hpp"

namespace CryptoNote {

class AmountFormatter : public IAmountFormatter {
 public:
  explicit AmountFormatter(const uint64_t decimals, std::string_view ticker,
                           const std::locale& locale = std::locale{""});
  ~AmountFormatter() override;

 public:
  std::string operator()(const uint64_t amount, bool addTicker = true) const override;
  std::string operator()(const int64_t amount, bool addTicker = true) const override;

  uint64_t decimals() const;
  const std::string& ticker() const;
  const std::locale& locale() const;

 private:
  uint64_t m_decimals;
  std::string m_ticker;
  std::locale m_locale;
};

}  // namespace CryptoNote
