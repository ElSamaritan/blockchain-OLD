// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

///////////////////////////////
#include <Transfer.h>
///////////////////////////////

#include <vector>
#include <utility>
#include <string>

#include <Xi/Algorithm/String.h>
#include <Xi/Time.h>

#include <Common/Base58.h>
#include <Common/StringTools.h>

#include <Xi/Config.h>

#include <CryptoNoteCore/CryptoNoteBasicImpl.h>
#include <CryptoNoteCore/CryptoNoteTools.h>
#include <CryptoNoteCore/Transactions/TransactionExtra.h>

#include "Wallet/IWallet.h"

/* NodeErrors.h and WalletErrors.h have some conflicting enums, e.g. they
   both export NOT_INITIALIZED, we can get round this by using a namespace */
namespace NodeErrors {
#include <NodeRpcProxy/NodeErrors.h>
}

#include <ColouredMsg.h>
#include <Fusion.h>
#include <Tools.h>
#include <Xi/Config/WalletConfig.h>

namespace WalletErrors {
#include <Wallet/WalletErrors.h>
}

#include <Wallet/WalletGreen.h>
#include <Wallet/WalletUtils.h>

bool parseAmount(std::string strAmount, const CryptoNote::Currency &currency, uint64_t &amount) {
  /* Trim any whitespace */
  trim(strAmount);

  /* If the user entered thousand separators, remove them */
  removeCharFromString(strAmount, ',');

  const size_t pointIndex = strAmount.find_first_of('.');
  const size_t numDecimalPlaces = currency.coin().decimals();

  size_t fractionSize;

  if (std::string::npos != pointIndex) {
    fractionSize = strAmount.size() - pointIndex - 1;

    while (numDecimalPlaces < fractionSize && '0' == strAmount.back()) {
      strAmount.erase(strAmount.size() - 1, 1);
      fractionSize--;
    }

    if (numDecimalPlaces < fractionSize) {
      return false;
    }

    strAmount.erase(pointIndex, 1);
  } else {
    fractionSize = 0;
  }

  if (strAmount.empty()) {
    return false;
  }

  if (!std::all_of(strAmount.begin(), strAmount.end(), ::isdigit)) {
    return false;
  }

  if (fractionSize < numDecimalPlaces) {
    strAmount.append(numDecimalPlaces - fractionSize, '0');
  }

  bool success = Common::fromString(strAmount, amount);

  if (!success) {
    return false;
  }

  return amount > 0;
}

bool confirmTransaction(CryptoNote::TransactionParameters t, std::shared_ptr<WalletInfo> walletInfo,
                        bool integratedAddress, uint64_t nodeFee, std::string originalAddress) {
  std::cout << std::endl << InformationMsg("Confirm Transaction?") << std::endl;

  std::cout << "You are sending " << SuccessMsg(walletInfo->currency().amountFormatter()(t.destinations[0].amount))
            << ", with a network fee of " << SuccessMsg(walletInfo->currency().amountFormatter()(t.fee)) << ","
            << std::endl
            << "and a node fee of " << SuccessMsg(walletInfo->currency().amountFormatter()(nodeFee));

  CryptoNote::PaymentId paymentId{CryptoNote::PaymentId::Null};
  CryptoNote::getPaymentIdFromTxExtra(t.extra, paymentId);

  /* Lets not split the integrated address out into its address and
     payment ID combo. It'll confused users. */
  if (!paymentId.isNull() && !integratedAddress) {
    std::cout << ", " << std::endl << "and a Payment ID of " << SuccessMsg(paymentId.toString());
  } else {
    std::cout << ".";
  }

  std::cout << " The transaction will be unlocked ";
  if (t.unlockTimestamp > 0) {
    if (t.unlockTimestamp > CryptoNote::BlockHeight::max().toIndex()) {
      std::cout << "at " << WarningMsg(Xi::Time::unixToLocalShortString(t.unlockTimestamp)) << ".";
    } else {
      std::cout << "at height " << WarningMsg(std::to_string(t.unlockTimestamp + 1)) << ".";
    }
  } else {
    std::cout << SuccessMsg("immediately.");
  }

  std::cout << std::endl
            << std::endl
            << "FROM: " << SuccessMsg(walletInfo->walletFileName) << std::endl
            << "TO: " << SuccessMsg(originalAddress) << std::endl
            << std::endl;

  if (confirm("Is this correct?")) {
    confirmPassword(walletInfo->walletPass);
    return true;
  }

  return false;
}

/* Note that the originalTXParams, and thus the splitTXParams already has the
   node transfer added */
void splitTX(CryptoNote::WalletGreen &wallet, const CryptoNote::TransactionParameters originalTXParams,
             uint64_t nodeFee) {
  std::cout << "Transaction is still too large to send, splitting into "
            << "multiple chunks." << std::endl
            << "It will slightly raise the fee you have to pay," << std::endl
            << "and hence reduce the total amount you can send if" << std::endl
            << "your balance cannot cover it." << std::endl;

  if (!confirm("Is this OK?")) {
    std::cout << WarningMsg("Cancelling transaction.") << std::endl;
    return;
  }

  uint64_t balance = wallet.getActualBalance();

  uint64_t totalAmount = originalTXParams.destinations[0].amount;
  uint64_t sentAmount = 0;
  uint64_t remainder = totalAmount - sentAmount;

  /* How much to split the remaining balance to be sent into each individual
     transaction. If it's 1, then we'll attempt to send the full amount,
     if it's 2, we'll send half, and so on. */
  uint64_t amountDivider = 1;

  int txNumber = 1;

  while (true) {
    auto splitTXParams = originalTXParams;

    splitTXParams.destinations[0].amount = totalAmount / amountDivider;

    /* If we have odd numbers, we can have an amount that is smaller
       than the remainder to send, but the remainder is less than
       2 * amount.
       So, we include this amount in our current transaction to prevent
       this change not being sent.
       If we're trying to send more than the remaining amount, set to
       the remaining amount. */
    if ((splitTXParams.destinations[0].amount != remainder && remainder < (splitTXParams.destinations[0].amount * 2)) ||
        (splitTXParams.destinations[0].amount > remainder)) {
      splitTXParams.destinations[0].amount = remainder;
    } else if (splitTXParams.destinations[0].amount + splitTXParams.fee + nodeFee > balance) {
      splitTXParams.destinations[0].amount = balance - splitTXParams.fee - nodeFee;
    }

    if (splitTXParams.destinations[0].amount == 0) {
      std::cout << WarningMsg("Failed to split up transaction, sorry.") << std::endl;

      return;
    }

    uint64_t totalNeeded = splitTXParams.destinations[0].amount + splitTXParams.fee + nodeFee;

    /* Need to update before checking intially */
    wallet.updateInternalCache();

    /* Balance is going to get locked as we send, wait for it to unlock
       and then send */
    while (wallet.getActualBalance() < totalNeeded) {
      std::cout << WarningMsg("Waiting for balance to unlock to send ") << WarningMsg("next transaction.") << std::endl
                << WarningMsg("Will try again in 15 seconds...") << std::endl
                << std::endl;

      std::this_thread::sleep_for(std::chrono::seconds(15));

      wallet.updateInternalCache();
    }

    auto preparedTransaction = wallet.formTransaction(splitTXParams);

    /* Still too large, increase divider and try again */
    if (wallet.txIsTooLarge(preparedTransaction)) {
      amountDivider++;

      /* This can take quite a long time getting mixins each time
         so let them know it's not frozen */
      std::cout << InformationMsg("Working...") << std::endl;

      continue;
    }

    std::cout << InformationMsg("Sending transaction number ") << InformationMsg(std::to_string(txNumber))
              << InformationMsg("...") << std::endl;

    const size_t id = wallet.transfer(preparedTransaction);
    auto hash = wallet.getTransaction(id).hash;

    std::stringstream stream;

    stream << "Transaction has been sent!" << std::endl
           << "Hash: " << Common::podToHex(hash) << std::endl
           << "Amount: " << wallet.currency().amountFormatter()(splitTXParams.destinations[0].amount) << std::endl
           << std::endl;

    std::cout << SuccessMsg(stream.str()) << std::endl;

    txNumber++;

    sentAmount += splitTXParams.destinations[0].amount;

    /* Remember to remove the fee and node fee as well from balance */
    balance -= splitTXParams.destinations[0].amount - splitTXParams.fee - nodeFee;

    remainder = totalAmount - sentAmount;

    /* We've sent the full amount required now */
    if (sentAmount == totalAmount) {
      std::cout << InformationMsg("All transactions have been sent!") << std::endl;

      return;
    }

    /* Went well, lets restart, trying to send the max amount */
    amountDivider = 1;
  }
}

void transfer(std::shared_ptr<WalletInfo> walletInfo, bool sendAll, CryptoNote::INode &node) {
  const auto feeAddress = node.feeAddress();
  uint64_t nodeFee = feeAddress ? feeAddress->amount : 0;

  std::cout << InformationMsg(
                   "Note: You can type cancel at any time to "
                   "cancel the transaction")
            << std::endl
            << std::endl;

  const uint64_t balance = walletInfo->wallet.getActualBalance();
  const uint64_t balanceNoDust = walletInfo->wallet.getBalanceMinusDust({});

  const auto maybeAddress = getAddress(
      "What address do you want to transfer"
      " to?: ",
      walletInfo->currency());

  if (!maybeAddress.isJust) {
    std::cout << WarningMsg("Cancelling transaction.") << std::endl;
    return;
  }

  /* We keep around the original entered address since we can't get back
     the original integratedAddress from extra, since payment ID's can
     be upper, lower, or mixed case, but they're only stored as lower in
     extra. We want this for the confirmation screen. */
  std::string originalAddress = maybeAddress.x.second;

  std::string address = originalAddress;

  CryptoNote::TransactionExtra extra{};

  bool integratedAddress = maybeAddress.x.first == IntegratedAddress;

  /* It's an integrated address, so lets extract out the true address and
     payment ID from the pair */
  if (integratedAddress) {
    auto addrPaymentIDPair = extractIntegratedAddress(maybeAddress.x.second, walletInfo->currency());
    address = addrPaymentIDPair.x.first;
    try {
      extra = getExtraFromPaymentID(addrPaymentIDPair.x.second);
    } catch (...) {
      std::cout << ErrorMsg("Invalid payment id encoded in integrated address: ") << WarningMsg(maybeAddress.x.second)
                << std::endl;
      return;
    }
  }

  /* Don't need to prompt for payment ID if they used an integrated
     address */
  if (!integratedAddress) {
    const auto maybeExtra = getExtra();

    if (!maybeExtra.isJust) {
      std::cout << WarningMsg("Cancelling transaction.") << std::endl;
      return;
    }

    extra = maybeExtra.x;
  }

  /* Make sure we set this later if we're sending everything by deducting
     the fee from full balance */
  uint64_t amount = 0;

  const uint64_t mixin = node.currency().mixinUpperBound(node.getLastKnownBlockVersion());

  /* If we're sending everything, obviously we don't need to ask them how
     much to send */
  if (!sendAll) {
    const auto maybeAmount = getTransferAmount(walletInfo->currency());

    if (!maybeAmount.isJust) {
      std::cout << WarningMsg("Cancelling transaction.") << std::endl;
      return;
    }

    amount = maybeAmount.x;

    switch (doWeHaveEnoughBalance(amount, node.currency().minimumFee(node.getLastKnownBlockVersion()), walletInfo,
                                  nodeFee)) {
      case NotEnoughBalance: {
        std::cout << WarningMsg("Cancelling transaction.") << std::endl;
        return;
      }
      case SetMixinToZero:
      default:
        break;
    }
  }

  const auto maybeFee =
      getFee(walletInfo->currency().minimumFee(node.getLastKnownBlockVersion()), walletInfo->currency());

  if (!maybeFee.isJust) {
    std::cout << WarningMsg("Cancelling transaction.") << std::endl;
    return;
  }

  const uint64_t fee = maybeFee.x;

  switch (doWeHaveEnoughBalance(amount, fee, walletInfo, nodeFee)) {
    case NotEnoughBalance: {
      std::cout << WarningMsg("Cancelling transaction.") << std::endl;
      return;
    }
    case SetMixinToZero:
    default:
      break;
  }

  const auto maybeUnlockTimestamp = getTransferUnlock(node.getLastKnownBlockVersion(), walletInfo->currency());
  if (!maybeUnlockTimestamp.isJust) {
    std::cout << WarningMsg("Cancelling transaction.") << std::endl;
    return;
  }
  uint64_t unlockTimestamp = maybeUnlockTimestamp.x;

  /* This doesn't account for dust. We should probably make a function to
     check for balance minus dust */
  if (sendAll) {
    if (balance != balanceNoDust) {
      uint64_t unsendable = balance - balanceNoDust;

      amount = balanceNoDust - fee - nodeFee;

      std::cout << WarningMsg("Due to dust inputs, we are unable to ") << WarningMsg("send ")
                << InformationMsg(walletInfo->currency().amountFormatter()(unsendable))
                << WarningMsg("of your balance.") << std::endl;
      return;
    } else {
      amount = balance - fee - nodeFee;
    }
  }

  doTransfer(address, amount, fee, extra, walletInfo, integratedAddress, mixin, feeAddress, originalAddress,
             unlockTimestamp, node.currency());
}

BalanceInfo doWeHaveEnoughBalance(uint64_t amount, uint64_t fee, std::shared_ptr<WalletInfo> walletInfo,
                                  uint64_t nodeFee) {
  const uint64_t balance = walletInfo->wallet.getActualBalance();
  /* They have to include at least a fee of this large */
  if (balance < amount + fee + nodeFee) {
    std::cout << std::endl
              << WarningMsg("You don't have enough funds to cover ") << WarningMsg("this transaction!") << std::endl
              << std::endl
              << "Funds needed: " << InformationMsg(walletInfo->currency().amountFormatter()(amount + fee + nodeFee))
              << " (Includes a network fee of " << InformationMsg(walletInfo->currency().amountFormatter()(fee))
              << " and a node fee of " << InformationMsg(walletInfo->currency().amountFormatter()(nodeFee)) << ")"
              << std::endl
              << "Funds available: " << SuccessMsg(walletInfo->currency().amountFormatter()(balance)) << std::endl
              << std::endl;

    return NotEnoughBalance;
  } else {
    return EnoughBalance;
  }
}

void doTransfer(std::string address, uint64_t amount, uint64_t fee, CryptoNote::TransactionExtra extra,
                std::shared_ptr<WalletInfo> walletInfo, bool integratedAddress, uint64_t mixin,
                std::optional<CryptoNote::FeeAddress> nodeFees, std::string originalAddress, uint64_t unlockTimestamp,
                const CryptoNote::Currency &currency) {
  const uint64_t balance = walletInfo->wallet.getActualBalance();
  const uint64_t nodeFee = nodeFees ? nodeFees->amount : 0;

  if (balance < amount + fee + nodeFee) {
    std::cout << WarningMsg("You don't have enough funds to cover this ") << WarningMsg("transaction!") << std::endl
              << InformationMsg("Funds needed: ")
              << InformationMsg(walletInfo->currency().amountFormatter()(amount + fee + nodeFee)) << std::endl
              << SuccessMsg("Funds available: " + walletInfo->currency().amountFormatter()(balance)) << std::endl;
    return;
  }

  CryptoNote::TransactionParameters p;

  p.destinations = std::vector<CryptoNote::WalletOrder>{{address, amount}};

  if (nodeFees && nodeFee > 0) {
    p.destinations.push_back({currency.accountAddressAsString(nodeFees->address), nodeFee});
  }

  p.fee = fee;
  p.mixIn = static_cast<uint16_t>(mixin);
  p.extra = extra;
  p.changeDestination = walletInfo->walletAddress;
  p.unlockTimestamp = unlockTimestamp;

  if (!confirmTransaction(p, walletInfo, integratedAddress, nodeFee, originalAddress)) {
    std::cout << WarningMsg("Cancelling transaction.") << std::endl;
    return;
  }

  sendTX(walletInfo, p, nodeFee);
}

void sendTX(std::shared_ptr<WalletInfo> walletInfo, CryptoNote::TransactionParameters p, uint64_t nodeFee) {
  try {
    auto tx = walletInfo->wallet.formTransaction(p);

    /* Transaction is too large. Lets try and perform fusion to let us
       send more at once */
    if (walletInfo->wallet.txIsTooLarge(tx)) {
      /* If the fusion transactions didn't completely unlock, abort tx */
      if (!fusionTX(walletInfo->wallet, p)) {
        return;
      }

      /* Reform with the optimized inputs */
      tx = walletInfo->wallet.formTransaction(p);

      /* If the transaction is still too large, lets split it up into
         smaller chunks */
      if (walletInfo->wallet.txIsTooLarge(tx)) {
        splitTX(walletInfo->wallet, p, nodeFee);
        return;
      }
    }

    const size_t id = walletInfo->wallet.transfer(tx);
    auto hash = walletInfo->wallet.getTransaction(id).hash;

    std::cout << SuccessMsg("Transaction has been sent!") << std::endl
              << SuccessMsg("Hash: ") << SuccessMsg(Common::podToHex(hash)) << std::endl;
  }
  /* Lets handle the error and possibly resend the transaction */
  catch (const std::system_error &e) {
    handleTransferError(e);
  } catch (const std::exception &e) {
    std::cout << WarningMsg("Exceptional behavious sending transaction: ") << WarningMsg(e.what());
  } catch (...) {
    std::cout << WarningMsg("Exceptional behavious sending transaction: UNKNOWN");
  }
}

void handleTransferError(const std::system_error &e) {
  const int errorCode = e.code().value();
  switch (errorCode) {
    case WalletErrors::CryptoNote::error::WRONG_AMOUNT:
    case WalletErrors::CryptoNote::error::MIXIN_COUNT_TOO_BIG:
    case NodeErrors::CryptoNote::error::INTERNAL_NODE_ERROR: {
      if (errorCode == WalletErrors::CryptoNote::error::WRONG_AMOUNT) {
        std::cout << WarningMsg(
                         "Failed to send transaction "
                         "- not enough funds!")
                  << std::endl
                  << "Unable to send dust inputs." << std::endl;
      } else {
        std::cout << WarningMsg("Failed to send transaction!") << std::endl
                  << "Unable to find enough outputs to "
                  << "mix with." << std::endl;
      }

      std::cout << "Try lowering the amount you are sending "
                << "in one transaction." << std::endl;
      std::cout << WarningMsg("Cancelling transaction.") << std::endl;

      break;
    }
    case NodeErrors::CryptoNote::error::NETWORK_ERROR:
    case NodeErrors::CryptoNote::error::CONNECT_ERROR: {
      std::cout << WarningMsg(
                       "Couldn't connect to the network "
                       "to send the transaction!")
                << std::endl
                << "Ensure " << WalletConfig::daemonName << " or the remote node you are using is open "
                << "and functioning." << std::endl;
      break;
    }
    default: {
      /* Some errors don't have an associated value, just an
         error string */
      std::string msg = e.what();

      if (msg == "Failed add key input: key image already spent") {
        std::cout << WarningMsg(
                         "Failed to send transaction - "
                         "wallet is not synced yet!")
                  << std::endl
                  << "Use the " << InformationMsg("bc_height") << " command to view the wallet sync status."
                  << std::endl;

        break;
      }

      std::cout << WarningMsg("Failed to send transaction!") << std::endl
                << "Error message: " << msg << std::endl
                << "Please report what you were doing to cause "
                << "this error so we can fix it! :)" << std::endl;
      break;
    }
  }
}

Maybe<CryptoNote::TransactionExtra> getPaymentID(std::string msg) {
  while (true) {
    std::string paymentID;

    std::cout << InformationMsg(msg) << WarningMsg("Warning: If you were given a payment ID,") << std::endl
              << WarningMsg("you MUST use it, or your funds may be lost!") << std::endl
              << "Hit enter for the default of no payment ID: ";

    std::getline(std::cin, paymentID);

    if (paymentID == "") {
      return Just<CryptoNote::TransactionExtra>(CryptoNote::TransactionExtra::Null);
    }

    if (paymentID == "cancel") {
      return Nothing<CryptoNote::TransactionExtra>();
    }

    CryptoNote::TransactionExtra extra;

    /* Convert the payment ID into an "extra" */
    if (!CryptoNote::createTxExtraWithPaymentId(paymentID, extra)) {
      std::cout << WarningMsg(
                       "Failed to parse! Payment ID's are 64 "
                       "character hexadecimal strings.")
                << std::endl;
      continue;
    }

    return Just<CryptoNote::TransactionExtra>(extra);
  }
}

CryptoNote::TransactionExtra getExtraFromPaymentID(std::string paymentID) {
  auto reval = CryptoNote::TransactionExtra::Null;
  if (!paymentID.empty()) {
    reval.paymentId = CryptoNote::PaymentId::fromString(paymentID).takeOrThrow();
  }
  return reval;
}

Maybe<CryptoNote::TransactionExtra> getExtra() {
  std::stringstream msg;

  msg << std::endl
      << "What payment ID do you want to use?" << std::endl
      << "These are usually used for sending to exchanges." << std::endl;

  auto maybePaymentID = getPaymentID(msg.str());

  if (!maybePaymentID.isJust) {
    return maybePaymentID;
  }

  if (maybePaymentID.x.isNull()) {
    return maybePaymentID;
  }

  return Just<CryptoNote::TransactionExtra>(maybePaymentID.x);
}

Maybe<uint64_t> getFee(const uint64_t minFee, const CryptoNote::Currency &currency) {
  while (true) {
    std::string stringAmount;
    std::cout << std::endl
              << InformationMsg("What fee do you want to use?") << std::endl
              << "Hit enter for the default fee of " << currency.amountFormatter()(minFee) << ": ";

    std::getline(std::cin, stringAmount);

    if (stringAmount == "") {
      return Just<uint64_t>(minFee);
    }

    if (stringAmount == "cancel") {
      return Nothing<uint64_t>();
    }

    uint64_t amount;

    if (parseFee(stringAmount, minFee, currency)) {
      parseAmount(stringAmount, currency, amount);
      return Just<uint64_t>(amount);
    }
  }
}

Maybe<uint64_t> getTransferAmount(const CryptoNote::Currency &currency) {
  while (true) {
    std::string stringAmount;

    std::cout << std::endl
              << InformationMsg("How much ") << InformationMsg(currency.coin().ticker())
              << InformationMsg(" do you want to send?: ");

    std::getline(std::cin, stringAmount);

    if (stringAmount == "cancel") {
      return Nothing<uint64_t>();
    }

    uint64_t amount;

    if (parseAmount(stringAmount, currency)) {
      parseAmount(stringAmount, currency, amount);
      return Just<uint64_t>(amount);
    }
  }
}

Maybe<uint64_t> getTransferUnlock(CryptoNote::BlockVersion version, const CryptoNote::Currency &currency) {
  while (true) {
    std::string userInput;
    std::cout
        << std::endl
        << InformationMsg(
               "Do you want to setup an unlock time? Transaction and the related change will be locked until the "
               "given height is reached.")
        << std::endl
        << InformationMsg(
               "Insert a blockchain height in the future to lock the coins and the change to that blockchain height to "
               "that height")
        << std::endl
        << InformationMsg("Hit enter for the default of no unlock time: ");

    std::getline(std::cin, userInput);

    if (userInput == "cancel") {
      return Nothing<uint64_t>();
    }

    uint64_t unlockTimestamp;
    if (parseTransferUnlock(userInput, version, currency, unlockTimestamp)) {
      return Just<uint64_t>(unlockTimestamp);
    }
  }
}

bool parseFee(std::string feeString, const uint64_t minFee, const CryptoNote::Currency &currency) {
  uint64_t fee;

  if (!parseAmount(feeString, currency, fee)) {
    std::cout << WarningMsg(
                     "Failed to parse fee! Ensure you entered the "
                     "value correctly.")
              << std::endl
              << "Please note, you can only use " << currency.coin().decimals() << " decimal places." << std::endl;

    return false;
  } else if (fee < minFee) {
    std::cout << WarningMsg("Fee must be at least ") << currency.amountFormatter()(minFee) << "!" << std::endl;

    return false;
  }

  return true;
}

Maybe<std::pair<std::string, std::string>> extractIntegratedAddress(std::string integratedAddress,
                                                                    const CryptoNote::Currency &currency) {
  if (integratedAddress.length() != WalletConfig::integratedAddressLength) {
    return Nothing<std::pair<std::string, std::string>>();
  }

  std::string decoded;
  uint64_t prefix;

  /* Need to be able to decode the string as an address */
  if (!Tools::Base58::decode_addr(integratedAddress, prefix, decoded)) {
    return Nothing<std::pair<std::string, std::string>>();
  }

  /* The prefix needs to be the same as the base58 prefix */
  if (prefix != currency.coin().prefix().base58()) {
    return Nothing<std::pair<std::string, std::string>>();
  }

  const uint64_t paymentIDLen = 64;

  /* Grab the payment ID from the decoded address */
  std::string paymentID = decoded.substr(0, paymentIDLen);

  /* The binary array encoded keys are the rest of the address */
  std::string keys = decoded.substr(paymentIDLen, std::string::npos);

  CryptoNote::AccountPublicAddress addr;
  CryptoNote::BinaryArray ba = Common::asBinaryArray(keys);

  if (!CryptoNote::fromBinaryArray(addr, ba)) {
    return Nothing<std::pair<std::string, std::string>>();
  }

  /* Parse the AccountPublicAddress into a standard wallet address */
  /* Use the calculated prefix from earlier for less typing :p */
  std::string address = CryptoNote::getAccountAddressAsStr(prefix, addr);

  /* The address out should of course be a valid address */
  if (!parseStandardAddress(address, currency)) {
    return Nothing<std::pair<std::string, std::string>>();
  }

  CryptoNote::TransactionExtra extra{};
  /* And the payment ID out should be valid as well! */
  if (!CryptoNote::createTxExtraWithPaymentId(paymentID, extra)) {
    return Nothing<std::pair<std::string, std::string>>();
  }

  return Just<std::pair<std::string, std::string>>({address, paymentID});
}

Maybe<std::pair<AddressType, std::string>> getAddress(std::string msg, const CryptoNote::Currency &currency) {
  while (true) {
    std::string address;

    std::cout << InformationMsg(msg);

    std::getline(std::cin, address);
    trim(address);

    if (address == "cancel") {
      return Nothing<std::pair<AddressType, std::string>>();
    }

    auto addressType = parseAddress(address, currency);

    if (addressType != NotAnAddress) {
      return Just<std::pair<AddressType, std::string>>({addressType, address});
    }
  }
}

AddressType parseAddress(std::string address, const CryptoNote::Currency &currency) {
  if (parseStandardAddress(address, currency)) {
    return StandardAddress;
  }

  if (parseIntegratedAddress(address, currency)) {
    return IntegratedAddress;
  }

  /* Failed to parse, lets try and diagnose a more accurate failure message */

  const auto prefix = currency.coin().prefix().text();
  const auto ticker = currency.coin().ticker();

  if (address.length() != WalletConfig::standardAddressLength &&
      address.length() != WalletConfig::integratedAddressLength) {
    std::cout << WarningMsg("Address is wrong length!") << std::endl
              << "It should be " << WalletConfig::standardAddressLength << " or "
              << WalletConfig::integratedAddressLength << " characters long, but it is " << address.length()
              << " characters long!" << std::endl
              << std::endl;

    return NotAnAddress;
  }

  if (address.substr(0, prefix.length()) != prefix) {
    std::cout << WarningMsg("Invalid address! It should start with ") << WarningMsg(prefix) << WarningMsg("!")
              << std::endl
              << std::endl;

    return NotAnAddress;
  }

  std::cout << WarningMsg("Failed to parse address, address is not a ") << WarningMsg("valid ") << WarningMsg(ticker)
            << WarningMsg(" address!") << std::endl
            << std::endl;

  return NotAnAddress;
}

bool parseIntegratedAddress(std::string integratedAddress, const CryptoNote::Currency &currency) {
  return extractIntegratedAddress(integratedAddress, currency).isJust;
}

bool parseStandardAddress(std::string address, const CryptoNote::Currency &currency, bool printErrors) {
  uint64_t prefix;

  CryptoNote::AccountPublicAddress addr;

  const auto addressPrefix = currency.coin().prefix().text();
  const auto ticker = currency.coin().ticker();

  const bool valid = CryptoNote::parseAccountAddressString(prefix, addr, address);

  if (address.length() != WalletConfig::standardAddressLength) {
    if (printErrors) {
      std::cout << WarningMsg("Address is wrong length!") << std::endl
                << "It should be " << WalletConfig::standardAddressLength << " characters long, but it is "
                << address.length() << " characters long!" << std::endl
                << std::endl;
    }

    return false;
  }
  /* We can't get the actual prefix if the address is invalid for other
     reasons. To work around this, we can just check that the address starts
     with XI, as long as the prefix is the XI prefix. This keeps it
     working on testnets with different prefixes. */
  else if (address.substr(0, addressPrefix.length()) != addressPrefix) {
    if (printErrors) {
      std::cout << WarningMsg("Invalid address! It should start with ") << WarningMsg(addressPrefix) << WarningMsg("!")
                << std::endl
                << std::endl;
    }

    return false;
  }
  /* We can return earlier by checking the value of valid, but then we don't
     get to give more detailed error messages about the address */
  else if (!valid) {
    if (printErrors) {
      std::cout << WarningMsg("Failed to parse address, address is not a ") << WarningMsg("valid ")
                << WarningMsg(ticker) << WarningMsg(" address!") << std::endl
                << std::endl;
    }

    return false;
  }

  return true;
}

bool parseAmount(std::string amountString, const CryptoNote::Currency &currency) {
  uint64_t amount;

  if (!parseAmount(amountString, currency, amount)) {
    std::cout << WarningMsg(
                     "Failed to parse amount! Ensure you entered "
                     "the value correctly.")
              << std::endl
              << "Please note, the minimum you can send is " << currency.amountFormatter()(static_cast<uint64_t>(1))
              << "," << std::endl
              << "and you can only use " << currency.coin().decimals() << " decimal places." << std::endl;

    return false;
  }

  return true;
}

bool parseTransferUnlock(const std::string &str, const CryptoNote::BlockVersion version,
                         const CryptoNote::Currency &currency, uint64_t &out) {
  try {
    auto trimmed = Xi::trim(str);
    if (trimmed.empty()) {
      out = 0;
      return true;
    } else {
      auto input = std::stoll(str);
      if (input < 1) {
        std::cout << WarningMsg("Unlock must be greater equal 1.") << std::endl;
        return false;
      } else {
        const auto unlock = static_cast<uint64_t>(input);
        if (unlock > currency.unlockLimit(version)) {
          std::cout << WarningMsg("Unlock exceeds limits: " + std::to_string(currency.unlockLimit(version)))
                    << std::endl;
          return false;
        }

        if (currency.isLockedBasedOnBlockIndex(unlock - 1)) {
          out = unlock - 1;
          return true;
        } else {
          if (unlock <= currency.coin().startTimestamp()) {
            std::cout << WarningMsg("Unlock is timestamp based but less than genesis timestamp: " +
                                    Xi::Time::unixToLocalShortString(currency.coin().startTimestamp()))
                      << std::endl;
            return false;
          } else {
            out = unlock;
            return true;
          }
        }
      }
    }
  } catch (...) {
    std::cout << WarningMsg("Failed to parse unlock. Pease ensure you enter a valid postive number.") << std::endl;
    return false;
  }
}
