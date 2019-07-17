// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include "Account.h"

#include "CryptoNoteSerialization.h"
#include "crypto/crypto.h"

namespace CryptoNote {
//-----------------------------------------------------------------
AccountBase::AccountBase() {
  nullify();
}
//-----------------------------------------------------------------
void AccountBase::nullify() {
  m_keys = AccountKeys();
}
//-----------------------------------------------------------------
void AccountBase::generate() {
  Crypto::generate_keys(m_keys.address.spendPublicKey, m_keys.spendSecretKey);

  /* We derive the view secret key by taking our spend secret key, hashing
     with keccak-256, and then using this as the seed to generate a new set
     of keys - the public and private view keys. See generate_deterministic_keys */

  generateViewFromSpend(m_keys.spendSecretKey, m_keys.viewSecretKey, m_keys.address.viewPublicKey);
  m_creation_timestamp = time(NULL);
}
void AccountBase::generateViewFromSpend(const Crypto::SecretKey &spend, Crypto::SecretKey &viewSecret,
                                        Crypto::PublicKey &viewPublic) {
  Crypto::SecretKey viewKeySeed;
  Crypto::generate_deterministic_keys(viewPublic, viewSecret, spend.span());
}

void AccountBase::generateViewFromSpend(const Crypto::SecretKey &spend, Crypto::SecretKey &viewSecret) {
  Crypto::generate_deterministic_keys(viewSecret, spend.span());
}
//-----------------------------------------------------------------
const AccountKeys &AccountBase::getAccountKeys() const {
  return m_keys;
}

void AccountBase::setAccountKeys(const AccountKeys &keys) {
  m_keys = keys;
}
//-----------------------------------------------------------------

bool AccountBase::serialize(ISerializer &s) {
  XI_RETURN_EC_IF_NOT(s(m_keys, "keys"), false);
  XI_RETURN_EC_IF_NOT(s(m_creation_timestamp, "creation_timestamp"), false);
  return true;
}
}  // namespace CryptoNote
