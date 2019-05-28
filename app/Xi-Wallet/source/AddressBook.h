// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include <string>
#include <cinttypes>

#include <Types.h>

void addToAddressBook();

void sendFromAddressBook(std::shared_ptr<WalletInfo> walletInfo, uint32_t height, const CryptoNote::Currency& currency,
                         std::optional<CryptoNote::FeeAddress> feeAddress = std::nullopt);

void deleteFromAddressBook();

void listAddressBook();

const Maybe<std::string> getAddressBookPaymentID();

const Maybe<const std::string> getAddressBookAddress();

const Maybe<const AddressBookEntry> getAddressBookEntry(AddressBook addressBook);

const std::string getAddressBookName(AddressBook addressBook);

AddressBook getAddressBook();

bool saveAddressBook(AddressBook addressBook);

bool isAddressBookEmpty(AddressBook addressBook);
