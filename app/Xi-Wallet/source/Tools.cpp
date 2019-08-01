// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

////////////////////////////
#include <Tools.h>
////////////////////////////

#include <cmath>

#include <Common/Base58.h>
#include <Common/StringTools.h>

#include <CryptoNoteCore/CryptoNoteBasicImpl.h>
#include <CryptoNoteCore/CryptoNoteTools.h>
#include <CryptoNoteCore/Transactions/TransactionExtra.h>

#include <fstream>
#include <locale>
#include <iostream>

#include <ColouredMsg.h>
#include <PasswordContainer.h>
#include <Xi/Config/WalletConfig.h>

void confirmPassword(const std::string &walletPass, const std::string &msg) {
  /* Password container requires an rvalue, we don't want to wipe our current
     pass so copy it into a tmp string and std::move that instead */
  std::string tmpString = walletPass;
  Tools::PasswordContainer pwdContainer(std::move(tmpString));

  while (!pwdContainer.read_and_validate(msg)) {
    std::cout << WarningMsg("Incorrect password! Try again.") << std::endl;
  }
}

bool confirm(const std::string &msg) { return confirm(msg, true); }

/* defaultReturn = what value we return on hitting enter, i.e. the "expected"
   workflow */
bool confirm(const std::string &msg, const bool defaultReturn) {
  /* In unix programs, the upper case letter indicates the default, for
     example when you hit enter */
  std::string prompt = " (Y/n): ";

  /* Yes, I know I can do !defaultReturn. It doesn't make as much sense
     though. If someone deletes this comment to make me look stupid I'll be
     mad >:( */
  if (defaultReturn == false) {
    prompt = " (y/N): ";
  }

  while (true) {
    std::cout << InformationMsg(msg + prompt);

    std::string answer;
    std::getline(std::cin, answer);

    const std::locale loc{};
    const char c = std::tolower(answer[0], loc);

    switch (c) {
      /* Lets people spam enter / choose default value */
      case '\0':
        return defaultReturn;
      case 'y':
        return true;
      case 'n':
        return false;
    }

    std::cout << WarningMsg("Bad input: ") << InformationMsg(answer) << WarningMsg(" - please enter either Y or N.")
              << std::endl;
  }
}

std::string unixTimeToDate(const uint64_t timestamp) {
  if (timestamp > static_cast<uint64_t>(std::numeric_limits<std::time_t>::max())) {
    return "INVALID";
  }
  const std::time_t time = static_cast<std::time_t>(timestamp);
  char buffer[100];
  std::strftime(buffer, sizeof(buffer), "%F %R", std::localtime(&time));
  return std::string(buffer);
}

std::string createIntegratedAddress(const std::string &address, const CryptoNote::Currency &currency,
                                    const std::string &paymentID) {
  uint64_t prefix;

  CryptoNote::AccountPublicAddress addr;

  /* Get the private + public key from the address */
  if (!CryptoNote::parseAccountAddressString(prefix, addr, address)) {
    throw std::runtime_error("invalid address provided");
  }

  /* Pack as a binary array */
  CryptoNote::BinaryArray ba = CryptoNote::toBinaryArray(addr);
  std::string keys = Common::asString(ba);

  /* Encode prefix + paymentID + keys as an address */
  return Tools::Base58::encode_addr(currency.coin().prefix().base58(), paymentID + keys);
}

CryptoNote::BlockHeight getScanHeight() {
  while (true) {
    std::cout << InformationMsg("What height would you like to begin ") << InformationMsg("scanning your wallet from?")
              << std::endl
              << std::endl
              << "This can greatly speed up the initial wallet "
              << "scanning process." << std::endl
              << std::endl
              << "If you do not know the exact height, "
              << "err on the side of caution so transactions do not "
              << "get missed." << std::endl
              << std::endl
              << InformationMsg("Hit enter for the sub-optimal default ") << InformationMsg("of zero: ");

    std::string stringHeight;

    std::getline(std::cin, stringHeight);

    /* Remove commas so user can enter height as e.g. 200,000 */
    removeCharFromString(stringHeight, ',');

    if (stringHeight == std::string{""}) {
      return CryptoNote::BlockHeight::Genesis;
    }

    try {
      const auto height = std::stoul(stringHeight);
      return height > 0 ? CryptoNote::BlockHeight::fromNative(static_cast<uint32_t>(height))
                        : CryptoNote::BlockHeight::Genesis;
    } catch (const std::invalid_argument &) {
      std::cout << WarningMsg("Failed to parse height - input is not ") << WarningMsg("a number!") << std::endl
                << std::endl;
    }
  }
}

/* Erases all instances of c from the string. E.g. 2,000,000 becomes 2000000 */
void removeCharFromString(std::string &str, const char c) {
  str.erase(std::remove(str.begin(), str.end(), c), str.end());
}

/* Trims any whitespace from left and right */
void trim(std::string &str) {
  rightTrim(str);
  leftTrim(str);
}

void leftTrim(std::string &str) {
  std::string whitespace = " \t\n\r\f\v";

  str.erase(0, str.find_first_not_of(whitespace));
}

void rightTrim(std::string &str) {
  std::string whitespace = " \t\n\r\f\v";

  str.erase(str.find_last_not_of(whitespace) + 1);
}

/* Checks if str begins with substring */
bool startsWith(const std::string &str, const std::string &substring) { return str.rfind(substring, 0) == 0; }

/* Does the given filename exist on disk? */
bool fileExists(const std::string &filename) {
  /* Bool conversion needs an explicit cast */
  return static_cast<bool>(std::ifstream(filename));
}

bool shutdown(std::shared_ptr<WalletInfo> walletInfo, CryptoNote::INode &node, bool &alreadyShuttingDown) {
  if (alreadyShuttingDown) {
    std::cout << "The shutdown process is already running." << std::endl;

    return false;
  }

  std::cout << InformationMsg("Shutting down...") << std::endl;

  alreadyShuttingDown = true;

  bool finishedShutdown = false;

  std::thread timelyShutdown([&finishedShutdown] {
    const auto startTime = std::chrono::system_clock::now();

    /* Has shutdown finished? */
    while (!finishedShutdown) {
      const auto currentTime = std::chrono::system_clock::now();

      /* If not, wait for a max of 20 seconds then force exit. */
      if ((currentTime - startTime) > std::chrono::seconds(20)) {
        std::cout << WarningMsg(
                         "Wallet took too long to save! "
                         "Force closing.")
                  << std::endl
                  << "Bye." << std::endl;
        exit(0);
      }

      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  });

  if (walletInfo != nullptr) {
    std::cout << InformationMsg("Saving wallet file...") << std::endl;

    walletInfo->wallet.save();

    std::cout << InformationMsg("Shutting down wallet interface...") << std::endl;

    walletInfo->wallet.shutdown();
  }

  std::cout << InformationMsg("Shutting down node connection...") << std::endl;

  node.shutdown();

  finishedShutdown = true;

  /* Wait for shutdown watcher to finish */
  timelyShutdown.join();

  std::cout << "Bye." << std::endl;

  return true;
}
