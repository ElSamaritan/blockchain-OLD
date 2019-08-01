// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include <string>
#include <cinttypes>

#include <Types.h>

void addToAddressBook(const CryptoNote::Currency &currency);

void sendFromAddressBook(std::shared_ptr<WalletInfo> walletInfo, CryptoNote::INode &node);

void deleteFromAddressBook();

void listAddressBook();

const Maybe<CryptoNote::TransactionExtra> getAddressBookPaymentID();

const Maybe<const std::string> getAddressBookAddress();

const Maybe<const AddressBookEntry> getAddressBookEntry(AddressBook addressBook);

const std::string getAddressBookName(AddressBook addressBook);

AddressBook getAddressBook();

bool saveAddressBook(AddressBook addressBook);

bool isAddressBookEmpty(AddressBook addressBook);
