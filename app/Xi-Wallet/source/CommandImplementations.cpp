// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

/////////////////////////////////////////////
#include <CommandImplementations.h>
/////////////////////////////////////////////

#include <atomic>
#include <string>
#include <vector>
#include <ctime>

#include <Xi/Algorithm/String.h>
#include <Common/FormatTools.h>
#include <Common/StringTools.h>

#include <CryptoNoteCore/Account.h>
#include <CryptoNoteCore/Transactions/TransactionExtra.h>

#include <Xi/ExternalIncludePush.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <Xi/ExternalIncludePop.h>

#ifndef MSVC
#include <fstream>
#endif

#include <Mnemonics/Mnemonics.h>

#include <AddressBook.h>
#include <ColouredMsg.h>
#include <Commands.h>
#include <Fusion.h>
#include <Menu.h>
#include <Open.h>
#include <Sync.h>
#include <Tools.h>
#include <Transfer.h>
#include <Types.h>
#include <Xi/Config/WalletConfig.h>

void changePassword(std::shared_ptr<WalletInfo> walletInfo) {
  /* Check the user knows the current password */
  confirmPassword(walletInfo->walletPass, "Confirm your current password: ");

  /* Get a new password for the wallet */
  const std::string newPassword = getWalletPassword(true, "Enter your new password: ");

  /* Change the wallet password */
  walletInfo->wallet.changePassword(walletInfo->walletPass, newPassword);

  /* Change the stored wallet metadata */
  walletInfo->walletPass = newPassword;

  /* Make sure we save with the new password */
  walletInfo->wallet.save();

  std::cout << SuccessMsg("Your password has been changed!") << std::endl;
}

void exportKeys(std::shared_ptr<WalletInfo> walletInfo) {
  confirmPassword(walletInfo->walletPass);
  printPrivateKeys(walletInfo->wallet, walletInfo->viewWallet);
}

void printPrivateKeys(CryptoNote::WalletGreen &wallet, bool viewWallet) {
  const Crypto::SecretKey privateViewKey = wallet.getViewKey().secretKey;

  if (viewWallet) {
    std::cout << SuccessMsg("Private view key:") << std::endl
              << SuccessMsg(Common::podToHex(privateViewKey)) << std::endl;
    return;
  }

  Crypto::SecretKey privateSpendKey = wallet.getAddressSpendKey(0).secretKey;

  Crypto::SecretKey derivedPrivateViewKey;

  CryptoNote::AccountBase::generateViewFromSpend(privateSpendKey, derivedPrivateViewKey);

  const bool deterministicPrivateKeys = derivedPrivateViewKey == privateViewKey;

  std::cout << SuccessMsg("Private spend key:") << std::endl
            << SuccessMsg(Common::podToHex(privateSpendKey)) << std::endl
            << std::endl
            << SuccessMsg("Private view key:") << std::endl
            << SuccessMsg(Common::podToHex(privateViewKey)) << std::endl;

  if (deterministicPrivateKeys) {
    std::cout << std::endl
              << SuccessMsg("Mnemonic seed:") << std::endl
              << SuccessMsg(Mnemonics::PrivateKeyToMnemonic(privateSpendKey)) << std::endl;
  }
}

void balance(CryptoNote::INode &node, CryptoNote::WalletGreen &wallet, bool viewWallet) {
  const uint64_t unconfirmedBalance = wallet.getPendingBalance();
  const uint64_t confirmedBalance = wallet.getActualBalance();
  const uint64_t totalBalance = unconfirmedBalance + confirmedBalance;

  const auto localHeight = node.getLastLocalBlockHeight();
  const auto remoteHeight = node.getLastKnownBlockHeight();
  const auto walletHeight = CryptoNote::BlockHeight::fromSize(wallet.getBlockCount());

  std::cout << "Available balance: " << SuccessMsg(formatAmount(confirmedBalance)) << std::endl
            << "Locked (unconfirmed) balance: " << WarningMsg(formatAmount(unconfirmedBalance)) << std::endl
            << "Total balance: " << InformationMsg(formatAmount(totalBalance)) << std::endl;

  if (viewWallet) {
    std::cout << std::endl
              << InformationMsg(
                     "Please note that view only wallets "
                     "can only track incoming transactions,")
              << std::endl
              << InformationMsg(
                     "and so your wallet balance may appear "
                     "inflated.")
              << std::endl;
  }

  if (localHeight < remoteHeight) {
    std::cout << std::endl
              << InformationMsg(
                     "Your daemon is not fully synced with "
                     "the network!")
              << std::endl
              << "Your balance may be incorrect until you are fully "
              << "synced!" << std::endl;
  }
  /* Small buffer because wallet height doesn't update instantly like node
     height does */
  else if (walletHeight.next(10) < remoteHeight) {
    std::cout << std::endl
              << InformationMsg(
                     "The blockchain is still being scanned for "
                     "your transactions.")
              << std::endl
              << "Balances might be incorrect whilst this is ongoing." << std::endl;
  }
}

void printHeights(CryptoNote::BlockHeight localHeight, CryptoNote::BlockHeight remoteHeight,
                  CryptoNote::BlockHeight walletHeight) {
  /* This is the height that the wallet has been scanned to. The blockchain
     can be fully updated, but we have to walk the chain to find our
     transactions, and this number indicates that progress. */
  std::cout << "Wallet blockchain height: ";

  /* Small buffer because wallet height doesn't update instantly like node
     height does */
  if (walletHeight.next(10) > remoteHeight) {
    std::cout << SuccessMsg(toString(walletHeight));
  } else {
    std::cout << WarningMsg(toString(walletHeight));
  }

  std::cout << std::endl << "Local blockchain height: ";

  if (localHeight == remoteHeight) {
    std::cout << SuccessMsg(toString(localHeight));
  } else {
    std::cout << WarningMsg(toString(localHeight));
  }

  std::cout << std::endl << "Network blockchain height: " << SuccessMsg(toString(remoteHeight)) << std::endl;
}

void printSyncStatus(CryptoNote::BlockHeight localHeight, CryptoNote::BlockHeight remoteHeight,
                     CryptoNote::BlockHeight walletHeight) {
  std::string networkSyncPercentage = Common::get_sync_percentage(localHeight, remoteHeight) + "%";

  std::string walletSyncPercentage = Common::get_sync_percentage(walletHeight, remoteHeight) + "%";

  std::cout << "Network sync status: ";

  if (localHeight == remoteHeight) {
    std::cout << SuccessMsg(networkSyncPercentage) << std::endl;
  } else {
    std::cout << WarningMsg(networkSyncPercentage) << std::endl;
  }

  std::cout << "Wallet sync status: ";

  /* Small buffer because wallet height is not always completely accurate */
  if (walletHeight.next(10) > remoteHeight) {
    std::cout << SuccessMsg(walletSyncPercentage) << std::endl;
  } else {
    std::cout << WarningMsg(walletSyncPercentage) << std::endl;
  }
}

void printSyncSummary(CryptoNote::BlockHeight localHeight, CryptoNote::BlockHeight remoteHeight,
                      CryptoNote::BlockHeight walletHeight) {
  if (localHeight.isNull() && remoteHeight.isNull()) {
    std::cout << WarningMsg("Uh oh, it looks like you don't have ") << WarningMsg(WalletConfig::daemonName)
              << WarningMsg(" open!") << std::endl;
  } else if (walletHeight.next(10) < remoteHeight && localHeight == remoteHeight) {
    std::cout << InformationMsg(
                     "You are synced with the network, but the "
                     "blockchain is still being scanned for "
                     "your transactions.")
              << std::endl
              << "Balances might be incorrect whilst this is ongoing." << std::endl;
  } else if (localHeight == remoteHeight) {
    std::cout << SuccessMsg("Yay! You are synced!") << std::endl;
  } else {
    std::cout << WarningMsg(
                     "Be patient, you are still syncing with the "
                     "network!")
              << std::endl;
  }
}

void printPeerCount(size_t peerCount) { std::cout << "Peers: " << SuccessMsg(std::to_string(peerCount)) << std::endl; }

void printHashrate(uint64_t difficulty) {
  /* Offline node / not responding */
  if (difficulty == 0) {
    return;
  }

  /* Hashrate is difficulty divided by block target time */
  uint32_t hashrate = static_cast<uint32_t>(round(difficulty / Xi::Config::Time::blockTimeSeconds()));

  std::cout << "Network hashrate: " << SuccessMsg(Common::get_mining_speed(hashrate))
            << " (Based on the last local block)" << std::endl;
}

/* This makes sure to call functions on the node which only return cached
   data. This ensures it returns promptly, and doesn't hang waiting for a
   response when the node is having issues. */
void status(CryptoNote::INode &node, CryptoNote::WalletGreen &wallet) {
  auto localHeight = node.getLastLocalBlockHeight();
  auto remoteHeight = node.getLastKnownBlockHeight();
  auto walletHeight = CryptoNote::BlockHeight::fromSize(wallet.getBlockCount());

  /* Print the heights of local, remote, and wallet */
  printHeights(localHeight, remoteHeight, walletHeight);

  std::cout << std::endl;

  /* Print the network and wallet sync status in percentage */
  printSyncStatus(localHeight, remoteHeight, walletHeight);

  std::cout << std::endl;

  /* Print the network hashrate, based on the last local block */
  printHashrate(node.getLastLocalBlockHeaderInfo().difficulty);

  /* Print the amount of peers we have */
  printPeerCount(node.getPeerCount());

  std::cout << std::endl;

  /* Print a summary of the sync status */
  printSyncSummary(localHeight, remoteHeight, walletHeight);
}

void reset(CryptoNote::INode &node, std::shared_ptr<WalletInfo> walletInfo) {
  auto scanHeight = getScanHeight();

  std::cout << std::endl
            << InformationMsg("This process may take some time to complete.") << std::endl
            << InformationMsg("You can't make any transactions during the ") << InformationMsg("process.") << std::endl
            << std::endl;

  if (!confirm("Are you sure?")) {
    return;
  }

  std::cout << InformationMsg("Resetting wallet...") << std::endl;

  walletInfo->wallet.reset(scanHeight);

  syncWallet(node, walletInfo);
}

void saveCSV(CryptoNote::WalletGreen &wallet, CryptoNote::INode &node) {
  XI_UNUSED(node);

  const size_t numTransactions = wallet.getTransactionCount();

  std::ofstream csv;
  csv.open(WalletConfig::csvFilename);

  if (!csv) {
    std::cout << WarningMsg(
                     "Couldn't open transactions.csv file for "
                     "saving!")
              << std::endl
              << WarningMsg(
                     "Ensure it is not open in any other "
                     "application.")
              << std::endl;
    return;
  }

  std::cout << InformationMsg("Saving CSV file...") << std::endl;

  /* Create CSV header */
  csv << "Timestamp,Block Height,Hash,Amount,In/Out" << std::endl;

  /* Loop through transactions */
  for (size_t i = 0; i < numTransactions; i++) {
    const CryptoNote::WalletTransaction t = wallet.getTransaction(i);

    /* Ignore fusion transactions */
    if (t.totalAmount == 0) {
      continue;
    }

    const std::string amount = formatAmountBasic(static_cast<uint64_t>(std::abs(t.totalAmount)));
    const std::string direction = t.totalAmount > 0 ? "IN" : "OUT";

    csv << unixTimeToDate(t.timestamp) << "," /* Timestamp */
        << toString(t.blockHeight) << ","     /* Block Height */
        << Common::podToHex(t.hash) << ","    /* Hash */
        << amount << ","                      /* Amount */
        << direction                          /* In/Out */
        << std::endl;
  }

  csv.close();

  std::cout << SuccessMsg("CSV successfully written to ") << SuccessMsg(WalletConfig::csvFilename) << SuccessMsg("!")
            << std::endl;
}

void printUnlockTime(CryptoNote::WalletTransaction t, CryptoNote::INode &node, bool isIncoming) {
  if (t.unlockTime > 0) {
    if (isIncoming) {
      std::cout << SuccessMsg("    Unlock        ");
    } else {
      std::cout << WarningMsg("    Unlock        ");
    }
    if (node.currency().isLockedBasedOnBlockIndex(t.unlockTime)) {
      if (isIncoming) {
        std::cout << SuccessMsg(std::to_string(t.unlockTime + 1));
      } else {
        std::cout << WarningMsg(std::to_string(t.unlockTime + 1));
      }
    } else {
      const auto timestamp = boost::posix_time::from_time_t(static_cast<std::time_t>(t.unlockTime));
      if (isIncoming) {
        std::cout << SuccessMsg(to_simple_string(timestamp));
      } else {
        std::cout << WarningMsg(to_simple_string(timestamp));
      }
    }

    if (node.currency().isUnlockSatisfied(t.unlockTime, node.getLastKnownBlockHeight().toIndex(),
                                          static_cast<uint64_t>(time(nullptr)))) {
      std::cout << SuccessMsg(" (UNLOCKED)");
    } else {
      std::cout << WarningMsg(" (LOCKED)");
    }
    std::cout << std::endl;
  }
}

void printOutgoingTransfer(CryptoNote::WalletTransaction t, CryptoNote::INode &node) {
  XI_UNUSED(node);
  assert(t.totalAmount < 0);
  const uint64_t absTotalAmount = static_cast<uint64_t>(std::abs(t.totalAmount));
  assert(t.fee <= absTotalAmount);

  std::cout << WarningMsg("Outgoing transfer:") << std::endl
            << WarningMsg("    Hash          " + Common::podToHex(t.hash)) << std::endl;

  if (t.timestamp != 0) {
    std::cout << WarningMsg("    Block height  ") << WarningMsg(toString(t.blockHeight)) << std::endl
              << WarningMsg("    Timestamp     ") << WarningMsg(unixTimeToDate(t.timestamp)) << std::endl;
  }
  printUnlockTime(t, node, false);

  std::cout << WarningMsg("    Spent         " + formatAmount(absTotalAmount - t.fee)) << std::endl
            << WarningMsg("    Fee           " + formatAmount(t.fee)) << std::endl
            << WarningMsg("    Total Spent   " + formatAmount(absTotalAmount)) << std::endl;

  const std::string paymentID = getPaymentIDFromExtra(t.extra);

  if (paymentID != "") {
    std::cout << WarningMsg("    Payment ID    " + paymentID) << std::endl;
  }

  std::cout << std::endl;
}

void printIncomingTransfer(CryptoNote::WalletTransaction t, CryptoNote::INode &node) {
  XI_UNUSED(node);
  assert(t.totalAmount > 0);
  const uint64_t absTotalAmount = static_cast<uint64_t>(std::abs(t.totalAmount));

  std::cout << SuccessMsg("Incoming transfer:") << std::endl
            << SuccessMsg("    Hash          " + Common::podToHex(t.hash)) << std::endl;

  if (t.timestamp != 0) {
    std::cout << SuccessMsg("    Block height  ") << SuccessMsg(toString(t.blockHeight)) << std::endl
              << SuccessMsg("    Timestamp     ") << SuccessMsg(unixTimeToDate(t.timestamp)) << std::endl;
  }
  printUnlockTime(t, node, true);

  std::cout << SuccessMsg("    Amount        " + formatAmount(absTotalAmount)) << std::endl;

  const std::string paymentID = getPaymentIDFromExtra(t.extra);

  if (paymentID != "") {
    std::cout << SuccessMsg("    Payment ID    " + paymentID) << std::endl;
  }

  std::cout << std::endl;
}

void listTransfers(bool incoming, bool outgoing, CryptoNote::WalletGreen &wallet, CryptoNote::INode &node) {
  const size_t numTransactions = wallet.getTransactionCount();

  int64_t totalSpent = 0;
  int64_t totalReceived = 0;

  for (size_t i = 0; i < numTransactions; i++) {
    const CryptoNote::WalletTransaction t = wallet.getTransaction(i);

    if (t.totalAmount < 0 && outgoing) {
      printOutgoingTransfer(t, node);
      totalSpent += -t.totalAmount;
    } else if (t.totalAmount > 0 && incoming) {
      printIncomingTransfer(t, node);
      totalReceived += t.totalAmount;
    }
  }

  if (incoming) {
    assert(totalReceived >= 0);
    std::cout << SuccessMsg("Total received: " + formatAmount(static_cast<uint64_t>(totalReceived))) << std::endl;
  }

  if (outgoing) {
    assert(totalSpent >= 0);
    std::cout << WarningMsg("Total spent: " + formatAmount(static_cast<uint64_t>(totalSpent))) << std::endl;
  }
}

void save(CryptoNote::WalletGreen &wallet) {
  std::cout << InformationMsg("Saving.") << std::endl;
  wallet.save();
  std::cout << InformationMsg("Saved.") << std::endl;
}

void createIntegratedAddress() {
  std::cout << InformationMsg("Creating an integrated address from an ")
            << InformationMsg("address and payment ID pair...") << std::endl
            << std::endl;

  std::string address;
  std::string paymentID;

  while (true) {
    std::cout << InformationMsg("Address: ");

    std::getline(std::cin, address);
    trim(address);

    std::cout << std::endl;

    if (parseStandardAddress(address, true)) {
      break;
    }
  }

  while (true) {
    std::cout << InformationMsg("Payment ID: ");

    std::getline(std::cin, paymentID);
    trim(paymentID);

    std::vector<uint8_t> extra;

    std::cout << std::endl;

    if (!CryptoNote::createTxExtraWithPaymentId(paymentID, extra)) {
      std::cout << WarningMsg(
                       "Failed to parse! Payment ID's are 64 "
                       "character hexadecimal strings.")
                << std::endl
                << std::endl;

      continue;
    }

    break;
  }

  std::cout << InformationMsg(createIntegratedAddress(address, paymentID)) << std::endl;
}

void help(std::shared_ptr<WalletInfo> wallet) {
  if (wallet->viewWallet) {
    printCommands(basicViewWalletCommands());
  } else {
    printCommands(basicCommands());
  }
}

void advanced(std::shared_ptr<WalletInfo> wallet) {
  /* We pass the offset of the command to know what index to print for
     command numbers */
  if (wallet->viewWallet) {
    printCommands(advancedViewWalletCommands(), static_cast<int>(basicViewWalletCommands().size()));
  } else {
    printCommands(advancedCommands(), static_cast<int>(basicCommands().size()));
  }
}

void generatePaymentId() {
  try {
    CryptoNote::PaymentId pid{};
    Crypto::SecretKey key{};
    Crypto::generate_keys(pid, key);
    std::cout << InformationMsg("PaymentId: ") << SuccessMsg(pid.toString()) << std::endl
              << std::endl
              << SuggestionMsg("You can proof your ownership of this payment id using the following secret key:")
              << std::endl
              << std::endl
              << SuccessMsg(key.toString()) << std::endl
              << std::endl
              << WarningMsg(
                     "Do not share your proof of ownership. If you are asked to proof your ownership use the "
                     "poof_payment_id_ownership command and provide the previous key.")
              << std::endl;
  } catch (std::exception &e) {
    std::cout << ErrorMsg() << "Failed to generate payment id: " << e.what() << std::endl;
  }
}

void proofPaymentIdOwnership() {
  try {
    while (true) {
    PROOF_PAYMENT_ID_OWNSERSHIP:
      std::cout << "Please provide the payment id key: " << std::endl;

      std::string line;
      std::getline(std::cin, line);
      if (line == "cancel") {
        break;
      }
      auto key = Crypto::SecretKey::fromString(Xi::trim(line));
      if (key.isError()) {
        std::cout << ErrorMsg(std::string{"Invalid key: "} + key.error().message()) << std::endl;
        goto PROOF_PAYMENT_ID_OWNSERSHIP;
      }
      ::Crypto::PublicKey pb{};
      if (!::Crypto::secret_key_to_public_key(*key, pb)) {
        std::cout << ErrorMsg("Invalid key: public key derivation failed.") << std::endl;
        goto PROOF_PAYMENT_ID_OWNSERSHIP;
      }

      std::cout << "Please provide a user provided string: " << std::endl;
      std::getline(std::cin, line);
      if (line == "cancel") {
        break;
      }

      line = Xi::trim(line);
      auto hash = ::Crypto::Hash::compute(Xi::asByteSpan(line)).takeOrThrow();
      ::Crypto::Signature sig{};
      ::Crypto::generate_signature(hash, pb, *key, sig);
      std::cout << InformationMsg("Signature generation succeeded:") << std::endl
                << SuccessMsg(sig.toString()) << std::endl
                << std::endl
                << InformationMsg("You can now securely send this proof to the user asking.") << std::endl;
      break;
    }
  } catch (std::exception &e) {
    std::cout << ErrorMsg(std::string{"Failed to proof payment id ownership: "} + e.what()) << std::endl;
  }
}

void checkPaymentIdOwnership() {
  try {
    while (true) {
    CHECK_PAYMENT_ID_OWNSERSHIP_ASK_FOR_PAYMENT_ID:
      std::cout << "Please provide the payment id: " << std::endl;
      std::string line;
      std::getline(std::cin, line);
      if (line == "cancel") {
        break;
      }

      auto pid = CryptoNote::PaymentId::fromString(Xi::trim(line));
      if (pid.isError()) {
        std::cout << ErrorMsg("Invalid payment id: " + pid.error().message()) << std::endl;
        goto CHECK_PAYMENT_ID_OWNSERSHIP_ASK_FOR_PAYMENT_ID;
      }

      std::cout << "Please provide your signing request: " << std::endl;
      std::getline(std::cin, line);
      if (line == "cancel") {
        break;
      }
      auto hash = ::Crypto::Hash::compute(Xi::asByteSpan(line)).takeOrThrow();

    CHECK_PAYMENT_ID_OWNSERSHIP_ASK_FOR_SIGNAUTRE:
      std::cout << "Please provide the signature returned: " << std::endl;
      std::getline(std::cin, line);
      if (line == "cancel") {
        break;
      }
      auto signature = ::Crypto::Signature::fromString(Xi::trim(line));
      if (signature.isError()) {
        std::cout << ErrorMsg("Invalid Signature: " + signature.error().message()) << std::endl;
        goto CHECK_PAYMENT_ID_OWNSERSHIP_ASK_FOR_SIGNAUTRE;
      }

      if (::Crypto::check_signature(hash, *pid, *signature)) {
        std::cout << SuccessMsg("VALID") << std::endl;
      } else {
        std::cout << ErrorMsg("INVALID") << std::endl;
      }
      break;
    }
  } catch (std::exception &e) {
    std::cout << ErrorMsg("Failed to check payment ownership: " + std::string{e.what()}) << std::endl;
  }
}
