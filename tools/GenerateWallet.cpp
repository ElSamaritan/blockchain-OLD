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

#include <iostream>

#include <Xi/Config/Coin.h>
#include <crypto/crypto.h>
#include <CryptoNoteCore/CryptoNote.h>
#include <CryptoNoteCore/CryptoNoteTools.h>
#include <CryptoNoteCore/Account.h>
#include <CryptoNoteCore/Currency.h>
#include <Serialization/JsonOutputStreamSerializer.h>
#include <Mnemonics/Mnemonics.h>
#include <Common/Base58.h>

int main(int, char**) {
  CryptoNote::AccountKeys account;
  Crypto::generate_keys(account.address.spendPublicKey, account.spendSecretKey);
  CryptoNote::AccountBase::generateViewFromSpend(account.spendSecretKey, account.viewSecretKey);
  if(!Crypto::secret_key_to_public_key(account.viewSecretKey, account.address.viewPublicKey)) {
      return -1;
  }

  auto binaryAddress = CryptoNote::toBinaryArray(account.address);
  auto address = Tools::Base58::encode_addr(0x1cf46e, Common::asString(binaryAddress));
  auto mnemonicSeed = Mnemonics::PrivateKeyToMnemonic(account.spendSecretKey);

  CryptoNote::JsonOutputStreamSerializer ser{};
  XI_RETURN_EC_IF_NOT(ser(address, "address"), -1);
  XI_RETURN_EC_IF_NOT(ser.beginObject("public_keys"), -1);
  XI_RETURN_EC_IF_NOT(ser(account.address.viewPublicKey, "view_key"),-1);
  XI_RETURN_EC_IF_NOT(ser(account.address.spendPublicKey, "spend_key"),-1);
  XI_RETURN_EC_IF_NOT(ser.endObject(), -1);
  XI_RETURN_EC_IF_NOT(ser.beginObject("secret_keys"), -1);
  XI_RETURN_EC_IF_NOT(ser(account.viewSecretKey, "view_key"), -1);
  XI_RETURN_EC_IF_NOT(ser(account.spendSecretKey, "spend_key"), -1);
  XI_RETURN_EC_IF_NOT(ser.endObject(), -1);
  XI_RETURN_EC_IF_NOT(ser(mnemonicSeed, "mnemonics"), -1);

  std::cout << ser << std::endl;

  return 0;
}
