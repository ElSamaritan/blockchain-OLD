// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include "WalletLegacySerializer.h"

#include <stdexcept>

#include <Xi/Crypto/Chacha8.h>
#include <Xi/Crypto/Random/Random.hh>

#include "Common/MemoryInputStream.h"
#include "Common/StdInputStream.h"
#include "Common/StdOutputStream.h"
#include "Serialization/BinaryOutputStreamSerializer.h"
#include "Serialization/BinaryInputStreamSerializer.h"
#include "CryptoNoteCore/Account.h"
#include "CryptoNoteCore/CryptoNoteSerialization.h"
#include "WalletLegacy/WalletUserTransactionsCache.h"
#include "Wallet/WalletErrors.h"
#include "Wallet/WalletUtils.h"
#include "WalletLegacy/KeysStorage.h"

using namespace Common;

namespace CryptoNote {

WalletLegacySerializer::WalletLegacySerializer(CryptoNote::AccountBase& account,
                                               WalletUserTransactionsCache& transactionsCache)
    : account(account), transactionsCache(transactionsCache), walletSerializationVersion(1) {}

bool WalletLegacySerializer::serialize(std::ostream& stream, const std::string& password, bool saveDetailed,
                                       const std::string& cache) {
  std::stringstream plainArchive;
  StdOutputStream plainStream(plainArchive);
  CryptoNote::BinaryOutputStreamSerializer serializer(plainStream);
  XI_RETURN_EC_IF_NOT(saveKeys(serializer), false);

  XI_RETURN_EC_IF_NOT(serializer(saveDetailed, "has_details"), false);

  if (saveDetailed) {
    XI_RETURN_EC_IF_NOT(serializer(transactionsCache, "details"), false);
  }

  XI_RETURN_EC_IF_NOT(serializer.binary(const_cast<std::string&>(cache), "cache"), false);

  std::string plain = plainArchive.str();
  std::string cipher;

  Crypto::chacha8_iv iv = encrypt(plain, password, cipher);

  uint32_t version = walletSerializationVersion;
  StdOutputStream output(stream);
  CryptoNote::BinaryOutputStreamSerializer s(output);
  XI_RETURN_EC_IF_NOT(s.beginObject("wallet"), false);
  XI_RETURN_EC_IF_NOT(s(version, "version"), false);
  XI_RETURN_EC_IF_NOT(s(iv, "iv"), false);
  XI_RETURN_EC_IF_NOT(s(cipher, "data"), false);
  s.endObject();

  stream.flush();
  return true;
}

bool WalletLegacySerializer::saveKeys(CryptoNote::ISerializer& serializer) {
  CryptoNote::KeysStorage keys;
  CryptoNote::AccountKeys acc = account.getAccountKeys();

  keys.creationTimestamp = account.get_createtime();
  keys.spendPublicKey = acc.address.spendPublicKey;
  keys.spendSecretKey = acc.spendSecretKey;
  keys.viewPublicKey = acc.address.viewPublicKey;
  keys.viewSecretKey = acc.viewSecretKey;

  return keys.serialize(serializer, "keys");
}

Crypto::chacha8_iv WalletLegacySerializer::encrypt(const std::string& plain, const std::string& password,
                                                   std::string& cipher) {
  Crypto::chacha8_key key;
  Xi::Crypto::Chacha8::generate_key(password, key.data, CHACHA8_KEY_SIZE);

  cipher.resize(plain.size());

  Crypto::chacha8_iv iv = Xi::Crypto::Random::generate<Crypto::chacha8_iv>();
  Crypto::chacha8(plain.data(), plain.size(), key, iv, &cipher[0]);

  return iv;
}

bool WalletLegacySerializer::deserialize(std::istream& stream, const std::string& password, std::string& cache) {
  StdInputStream stdStream(stream);
  CryptoNote::BinaryInputStreamSerializer serializerEncrypted(stdStream);

  XI_RETURN_EC_IF_NOT(serializerEncrypted.beginObject("wallet"), false);

  uint32_t version;
  XI_RETURN_EC_IF_NOT(serializerEncrypted(version, "version"), false);

  Crypto::chacha8_iv iv;
  XI_RETURN_EC_IF_NOT(serializerEncrypted(iv, "iv"), false);

  std::string cipher;
  XI_RETURN_EC_IF_NOT(serializerEncrypted(cipher, "data"), false);

  serializerEncrypted.endObject();

  std::string plain;
  decrypt(cipher, plain, iv, password);

  MemoryInputStream decryptedStream(plain.data(), plain.size());
  CryptoNote::BinaryInputStreamSerializer serializer(decryptedStream);

  XI_RETURN_EC_IF_NOT(loadKeys(serializer), false);
  throwIfKeysMismatch(account.getAccountKeys().viewSecretKey, account.getAccountKeys().address.viewPublicKey);

  if (!account.getAccountKeys().spendSecretKey.isNull()) {
    throwIfKeysMismatch(account.getAccountKeys().spendSecretKey, account.getAccountKeys().address.spendPublicKey);
  } else {
    if (!Crypto::check_key(account.getAccountKeys().address.spendPublicKey)) {
      throw std::system_error(make_error_code(CryptoNote::error::WRONG_PASSWORD));
    }
  }

  bool detailsSaved;

  XI_RETURN_EC_IF_NOT(serializer(detailsSaved, "has_details"), false);

  if (detailsSaved) {
    XI_RETURN_EC_IF_NOT(serializer(transactionsCache, "details"), false);
  }

  XI_RETURN_EC_IF_NOT(serializer.binary(cache, "cache"), false);
  return true;
}

void WalletLegacySerializer::decrypt(const std::string& cipher, std::string& plain, Crypto::chacha8_iv iv,
                                     const std::string& password) {
  Crypto::chacha8_key key;
  Xi::Crypto::Chacha8::generate_key(password, key.data, CHACHA8_KEY_SIZE);

  plain.resize(cipher.size());

  Crypto::chacha8(cipher.data(), cipher.size(), key, iv, &plain[0]);
}

bool WalletLegacySerializer::loadKeys(CryptoNote::ISerializer& serializer) {
  CryptoNote::KeysStorage keys;

  try {
    XI_RETURN_EC_IF_NOT(keys.serialize(serializer, "keys"), false);
  } catch (const std::runtime_error&) {
    throw std::system_error(make_error_code(CryptoNote::error::WRONG_PASSWORD));
  }

  CryptoNote::AccountKeys acc;
  acc.address.spendPublicKey = keys.spendPublicKey;
  acc.spendSecretKey = keys.spendSecretKey;
  acc.address.viewPublicKey = keys.viewPublicKey;
  acc.viewSecretKey = keys.viewSecretKey;

  account.setAccountKeys(acc);
  account.set_createtime(keys.creationTimestamp);
  return true;
}

}  // namespace CryptoNote
