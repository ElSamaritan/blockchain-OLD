// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

//////////////////////////////////
#include <AddressBook.h>
//////////////////////////////////

#ifndef MSVC
#include <fstream>
#endif

#include <string>

#include <Serialization/SerializationTools.h>

#include <Wallet/WalletUtils.h>

#include <ColouredMsg.h>
#include <Tools.h>
#include <Transfer.h>
#include <Xi/Config/WalletConfig.h>

const std::string getAddressBookName(AddressBook addressBook) {
  while (true) {
    std::string friendlyName;

    std::cout << InformationMsg("What friendly name do you want to ")
              << InformationMsg("give this address book entry?: ");

    std::getline(std::cin, friendlyName);
    trim(friendlyName);

    const auto it = std::find(addressBook.begin(), addressBook.end(), AddressBookEntry(friendlyName));

    if (it != addressBook.end()) {
      std::cout << WarningMsg("An address book entry with this ") << WarningMsg("name already exists!") << std::endl
                << std::endl;

      continue;
    }

    return friendlyName;
  }
}

const Maybe<std::string> getAddressBookPaymentID() {
  std::stringstream msg;

  msg << std::endl << "Does this address book entry have a payment ID associated with it?" << std::endl;

  return getPaymentID(msg.str());
}

void addToAddressBook() {
  std::cout << InformationMsg(
                   "Note: You can type cancel at any time to "
                   "cancel adding someone to your address book")
            << std::endl
            << std::endl;

  auto addressBook = getAddressBook();

  const std::string friendlyName = getAddressBookName(addressBook);

  if (friendlyName == "cancel") {
    std::cout << WarningMsg("Cancelling addition to address book.") << std::endl;
    return;
  }

  auto maybeAddress = getAddress("\nWhat address does this user have? ");

  if (!maybeAddress.isJust) {
    std::cout << WarningMsg("Cancelling addition to address book.") << std::endl;
    return;
  }

  std::string address = maybeAddress.x.second;
  std::string paymentID = "";

  bool integratedAddress = maybeAddress.x.first == IntegratedAddress;

  if (!integratedAddress) {
    auto maybePaymentID = getAddressBookPaymentID();

    if (!maybePaymentID.isJust) {
      std::cout << WarningMsg("Cancelling addition to address book.") << std::endl;

      return;
    }

    paymentID = maybePaymentID.x;
  }

  addressBook.emplace_back(friendlyName, address, paymentID, integratedAddress);

  if (saveAddressBook(addressBook)) {
    std::cout << std::endl
              << SuccessMsg("A new entry has been added to your address ") << SuccessMsg("book!") << std::endl;
  }
}

const Maybe<const AddressBookEntry> getAddressBookEntry(AddressBook addressBook) {
  while (true) {
    std::string friendlyName;

    std::cout << InformationMsg("Who do you want to send to from your ") << InformationMsg("address book?: ");

    std::getline(std::cin, friendlyName);
    trim(friendlyName);

    if (friendlyName == "cancel") {
      return Nothing<const AddressBookEntry>();
    }

    auto it = std::find(addressBook.begin(), addressBook.end(), AddressBookEntry(friendlyName));

    if (it != addressBook.end()) {
      return Just<const AddressBookEntry>(*it);
    }

    std::cout << std::endl
              << WarningMsg("Could not find a user with the name of ") << InformationMsg(friendlyName)
              << WarningMsg(" in your address book!") << std::endl
              << std::endl;

    const bool list = confirm(
        "Would you like to list everyone in your "
        "address book?");

    std::cout << std::endl;

    if (list) {
      listAddressBook();
    }
  }
}

void sendFromAddressBook(std::shared_ptr<WalletInfo> walletInfo, CryptoNote::INode& node) {
  auto addressBook = getAddressBook();

  if (isAddressBookEmpty(addressBook)) {
    return;
  }

  std::cout << InformationMsg("Note: You can type cancel at any time to ") << InformationMsg("cancel the transaction")
            << std::endl
            << std::endl;

  auto maybeAddressBookEntry = getAddressBookEntry(addressBook);

  if (!maybeAddressBookEntry.isJust) {
    std::cout << WarningMsg("Cancelling transaction.") << std::endl;
    return;
  }

  auto addressBookEntry = maybeAddressBookEntry.x;

  auto maybeAmount = getTransferAmount();

  if (!maybeAmount.isJust) {
    std::cout << WarningMsg("Cancelling transction.") << std::endl;
    return;
  }

  const auto maybeUnlockTimestamp = getUnlockTimestamp();
  if (!maybeUnlockTimestamp.isJust) {
    std::cout << WarningMsg("Cancelling transaction.") << std::endl;
    return;
  }
  uint64_t unlockTimestamp = maybeUnlockTimestamp.x;

  /* Originally entered address, so we can preserve the correct integrated
     address for confirmation screen */
  auto originalAddress = addressBookEntry.address;
  auto address = originalAddress;
  auto amount = maybeAmount.x;
  auto fee = node.currency().minimumFee(node.getLastKnownBlockVersion());
  auto extra = getExtraFromPaymentID(addressBookEntry.paymentID);
  auto mixin = node.currency().mixinUpperBound(node.getLastKnownBlockVersion());
  auto integrated = addressBookEntry.integratedAddress;

  if (integrated) {
    auto addrPaymentIDPair = extractIntegratedAddress(address);
    address = addrPaymentIDPair.x.first;
    extra = getExtraFromPaymentID(addrPaymentIDPair.x.second);
  }
  doTransfer(address, amount, fee, extra, walletInfo, integrated, mixin, node.feeAddress(), originalAddress,
             unlockTimestamp, node.currency());
}

bool isAddressBookEmpty(AddressBook addressBook) {
  if (addressBook.empty()) {
    std::cout << WarningMsg("Your address book is empty! Add some people ") << WarningMsg("to it first.") << std::endl;

    return true;
  }

  return false;
}

void deleteFromAddressBook() {
  auto addressBook = getAddressBook();

  if (isAddressBookEmpty(addressBook)) {
    return;
  }

  while (true) {
    std::cout << InformationMsg("Note: You can type cancel at any time ") << InformationMsg("to cancel the deletion")
              << std::endl
              << std::endl;

    std::string friendlyName;

    std::cout << InformationMsg("What address book entry do you want to ") << InformationMsg("delete?: ");

    std::getline(std::cin, friendlyName);
    trim(friendlyName);

    if (friendlyName == "cancel") {
      std::cout << WarningMsg("Cancelling deletion.") << std::endl;
      return;
    }

    auto it = std::find(addressBook.begin(), addressBook.end(), AddressBookEntry(friendlyName));

    if (it != addressBook.end()) {
      addressBook.erase(it);

      if (saveAddressBook(addressBook)) {
        std::cout << std::endl
                  << SuccessMsg("This entry has been deleted from ") << SuccessMsg("your address book!") << std::endl;
      }

      return;
    }

    std::cout << std::endl
              << WarningMsg("Could not find a user with the name of ") << InformationMsg(friendlyName)
              << WarningMsg(" in your address book!") << std::endl
              << std::endl;

    bool list = confirm(
        "Would you like to list everyone in your "
        "address book?");

    std::cout << std::endl;

    if (list) {
      listAddressBook();
    }
  }
}

void listAddressBook() {
  auto addressBook = getAddressBook();

  if (isAddressBookEmpty(addressBook)) {
    return;
  }

  int index = 1;

  for (const auto& i : addressBook) {
    std::cout << InformationMsg("Address Book Entry #") << InformationMsg(std::to_string(index)) << InformationMsg(":")
              << std::endl
              << std::endl
              << InformationMsg("Friendly Name: ") << std::endl
              << SuccessMsg(i.friendlyName) << std::endl
              << std::endl
              << InformationMsg("Address: ") << std::endl
              << SuccessMsg(i.address) << std::endl
              << std::endl;

    if (i.paymentID != "") {
      std::cout << InformationMsg("Payment ID: ") << std::endl
                << SuccessMsg(i.paymentID) << std::endl
                << std::endl
                << std::endl;
    } else {
      std::cout << std::endl;
    }

    index++;
  }
}

AddressBook getAddressBook() {
  AddressBook addressBook;

  std::ifstream input(WalletConfig::addressBookFilename);

  /* If file exists, read current values */
  if (input) {
    std::stringstream buffer;
    buffer << input.rdbuf();
    input.close();

    if (!CryptoNote::loadFromJson(addressBook, buffer.str())) {
      throw std::runtime_error("failed to read address book file.");
    }
  }

  return addressBook;
}

bool saveAddressBook(AddressBook addressBook) {
  std::string jsonString = CryptoNote::storeToJson(addressBook);

  std::ofstream output(WalletConfig::addressBookFilename);

  if (output) {
    output << jsonString;
  } else {
    std::cout << WarningMsg("Failed to save address book to disk!") << std::endl
              << WarningMsg("Check you are able to write files to your ") << WarningMsg("current directory.")
              << std::endl;

    return false;
  }

  output.close();

  return true;
}
