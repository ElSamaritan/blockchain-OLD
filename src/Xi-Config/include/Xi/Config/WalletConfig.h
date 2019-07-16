#pragma once

#include <string>
#include <cinttypes>

#include <Xi/Config.h>

/* Make sure everything in here is const - or it won't compile! */
namespace WalletConfig {

/* The filename to output the CSV to in save_csv */
const std::string csvFilename = "transactions.csv";

/* The filename to read+write the address book to - consider starting with
   a leading '.' to make it hidden under mac+linux */
const std::string addressBookFilename = ".addressBook.json";

/* The name of your deamon */
const std::string daemonName = "xi-daemon";

/* The name to call this wallet */
const std::string walletName = "xi-wallet";

/* The name of service/walletd, the programmatic rpc interface to a
   wallet */
const std::string walletdName = "xi-pgservice";

/* The length of a standard address for your coin */
const long unsigned int standardAddressLength = 98;

/* The length of an integrated address for your coin - It's the same as
   a normal address, but there is a paymentID included in there - since
   payment ID's are 64 chars, and base58 encoding is done by encoding
   chunks of 8 chars at once into blocks of 11 chars, we can calculate
   this automatically */
const long unsigned int integratedAddressLength = standardAddressLength + ((64 * 11) / 8);
}  // namespace WalletConfig
