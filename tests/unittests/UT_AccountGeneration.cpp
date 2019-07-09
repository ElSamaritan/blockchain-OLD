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

#include <gtest/gtest.h>

#include <crypto/crypto.h>
#include <CryptoNoteCore/CryptoNote.h>
#include <CryptoNoteCore/Account.h>
#include <CryptoNoteCore/Currency.h>

TEST(CryptoNote, AccountGeneration) {
  CryptoNote::AccountKeys account;
  Crypto::generate_keys(account.address.spendPublicKey, account.spendSecretKey);
  CryptoNote::AccountBase::generateViewFromSpend(account.spendSecretKey, account.viewSecretKey);
  EXPECT_TRUE(Crypto::secret_key_to_public_key(account.viewSecretKey, account.address.viewPublicKey));

  auto currency = CryptoNote::CurrencyBuilder(Logging::noLogging()).currency();
  auto address = currency.accountAddressAsString(account.address);

  CryptoNote::AccountPublicAddress recoveredAddress;
  ASSERT_TRUE(currency.parseAccountAddressString(address, recoveredAddress));

  EXPECT_EQ(account.address.viewPublicKey, recoveredAddress.viewPublicKey);
  EXPECT_EQ(account.address.spendPublicKey, recoveredAddress.spendPublicKey);
}
