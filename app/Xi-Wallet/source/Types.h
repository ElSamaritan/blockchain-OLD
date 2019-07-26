// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <string>
#include <vector>
#include <cinttypes>

#include <Xi/Config.h>

#include <Xi/Http/SSLConfiguration.h>

#include <Serialization/ISerializer.h>
#include <Serialization/SerializationTools.h>
#include <Common/JsonValue.h>
#include <Common/Util.h>
#include <Wallet/WalletGreen.h>

struct WalletInfo {
  WalletInfo(std::string walletFileName, std::string walletPass, std::string walletAddress, bool viewWallet,
             CryptoNote::WalletGreen &wallet)
      : walletFileName(walletFileName),
        walletPass(walletPass),
        walletAddress(walletAddress),
        viewWallet(viewWallet),
        wallet(wallet) {}

  /* How many transactions do we know about */
  size_t knownTransactionCount = 0;

  /* The wallet file name */
  std::string walletFileName;

  /* The wallet password */
  std::string walletPass;

  /* The wallets primary address */
  std::string walletAddress;

  /* Is the wallet a view only wallet */
  bool viewWallet;

  /* The walletgreen wallet container */
  CryptoNote::WalletGreen &wallet;

  const CryptoNote::Currency &currency() { return this->wallet.currency(); }
};

struct Config {
  /* Was the wallet file specified on CLI */
  bool walletGiven = false;

  /* Was the wallet pass specified on CLI */
  bool passGiven = false;

  /* Should we log walletd logs to a file */
  bool debug = false;

  /* Enables verbose logging to standard output buffer */
  bool verbose = false;

  std::string dataDir = Tools::getDefaultDataDirectory();
  /* The daemon host */
  std::string host = "";
  /* The daemon host access token, if required */
  std::string remoteRpcAccessToken = "";

  /* The network to connect to, ie. MainNet/StageNet ... */
  std::string network = Xi::Config::Network::defaultNetworkIdentifier();
  std::string networkDir = "./config";

  /* The daemon port */
  uint16_t port = Xi::Config::Network::Configuration::rpcDefaultPort();

  /* The wallet file path */
  std::string walletFile = "";

  /* The wallet password */
  std::string walletPass = "";

  /* Generates a wallet and exits. */
  bool generate = false;
  /* Uses a seed for wallet generation, can be used to recover wallets. */
  std::string generate_seed{};

  /* Configuration of SSL usage using the HTTP(S) client. */
  ::Xi::Http::SSLConfiguration ssl;
};

struct AddressBookEntry {
  AddressBookEntry() {}

  /* Used for quick comparison with strings */
  AddressBookEntry(std::string friendlyName) : friendlyName(friendlyName) {}

  AddressBookEntry(std::string friendlyName, std::string address, std::string paymentID, bool integratedAddress)
      : friendlyName(friendlyName), address(address), paymentID(paymentID), integratedAddress(integratedAddress) {}

  /* Friendly name for this address book entry */
  std::string friendlyName;

  /* The wallet address of this entry */
  std::string address;

  /* The payment ID associated with this address */
  std::string paymentID;

  /* Did the user enter this as an integrated address? (We need this to
     display back the address as either an integrated address, or an
     address + payment ID pair */
  bool integratedAddress;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(friendlyName, friendly_name)
  KV_MEMBER_RENAME(address, address)
  KV_MEMBER_RENAME(paymentID, payment_id)
  KV_MEMBER_RENAME(integratedAddress, integrated_address)
  KV_END_SERIALIZATION

  /* Only compare via name as we don't really care about the contents */
  bool operator==(const AddressBookEntry &rhs) const { return rhs.friendlyName == friendlyName; }
};

/* An address book is a vector of address book entries */
typedef std::vector<AddressBookEntry> AddressBook;

/* This borrows from haskell, and is a nicer boost::optional class. We either
   have Just a value, or Nothing.

   Example usage follows.
   The below code will print:

   ```
   100
   Nothing
   ```

   Maybe<int> parseAmount(std::string input)
   {
        if (input.length() == 0)
        {
            return Nothing<int>();
        }

        try
        {
            return Just<int>(std::stoi(input)
        }
        catch (const std::invalid_argument &)
        {
            return Nothing<int>();
        }
   }

   int main()
   {
       auto foo = parseAmount("100");

       if (foo.isJust)
       {
           std::cout << foo.x << std::endl;
       }
       else
       {
           std::cout << "Nothing" << std::endl;
       }

       auto bar = parseAmount("garbage");

       if (bar.isJust)
       {
           std::cout << bar.x << std::endl;
       }
       else
       {
           std::cout << "Nothing" << std::endl;
       }
   }

*/

template <class X>
struct Maybe {
  X x;
  bool isJust;

  Maybe(const X &x) : x(x), isJust(true) {}
  Maybe() : isJust(false) {}
};

template <class X>
Maybe<X> Just(const X &x) {
  return Maybe<X>(x);
}

template <class X>
Maybe<X> Nothing() {
  return Maybe<X>();
}

namespace CryptoNote {
template <>
inline Common::JsonValue storeContainerToJsonValue(const std::vector<AddressBookEntry> &cont) {
  Common::JsonValue js(Common::JsonValue::ARRAY);
  for (const auto &item : cont) {
    js.pushBack(storeToJsonValue(item));
  }
  return js;
}

template <>
inline void loadFromJsonValue(AddressBook &v, const Common::JsonValue &js) {
  for (size_t i = 0; i < js.size(); ++i) {
    AddressBookEntry type;
    loadFromJsonValue(type, js[i]);
    v.push_back(type);
  }
}
}  // namespace CryptoNote
