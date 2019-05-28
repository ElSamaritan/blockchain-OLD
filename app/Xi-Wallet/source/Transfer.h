// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <memory>
#include <utility>
#include <string>

#include <Types.h>
#include <Xi/Config/WalletConfig.h>

enum AddressType { NotAnAddress, IntegratedAddress, StandardAddress };

enum BalanceInfo { NotEnoughBalance, EnoughBalance, SetMixinToZero };

void transfer(std::shared_ptr<WalletInfo> walletInfo, uint32_t height, const CryptoNote::Currency& currency,
              bool sendAll = false, std::optional<CryptoNote::FeeAddress> feeAddress = std::nullopt);

void doTransfer(std::string address, uint64_t amount, uint64_t fee, std::string extra,
                std::shared_ptr<WalletInfo> walletInfo, uint32_t height, bool integratedAddress, uint64_t mixin,
                std::optional<CryptoNote::FeeAddress> nodeFees, std::string originalAddress, uint64_t unlockTimestamp,
                const CryptoNote::Currency& currency);

void splitTX(CryptoNote::WalletGreen& wallet, const CryptoNote::TransactionParameters splitTXParams, uint64_t nodeFee);

void sendTX(std::shared_ptr<WalletInfo> walletInfo, CryptoNote::TransactionParameters p, uint32_t height,
            bool retried = false, uint64_t nodeFee = 0);

bool confirmTransaction(CryptoNote::TransactionParameters t, std::shared_ptr<WalletInfo> walletInfo,
                        bool integratedAddress, uint64_t nodeFee, std::string originalAddress);

bool parseAmount(std::string amountString);

bool parseStandardAddress(std::string address, bool printErrors = false);

bool parseIntegratedAddress(std::string address);

bool parseFee(std::string feeString);

bool handleTransferError(const std::system_error& e, bool retried, uint32_t height);

AddressType parseAddress(std::string address);

std::string getExtraFromPaymentID(std::string paymentID);

Maybe<std::string> getPaymentID(std::string msg);

Maybe<std::string> getExtra();

Maybe<std::pair<AddressType, std::string>> getAddress(std::string msg);

Maybe<uint64_t> getFee();

Maybe<uint64_t> getTransferAmount();

Maybe<uint64_t> getUnlockTimestamp();

bool parseUnlockTimestamp(const std::string& str, uint64_t& out);

Maybe<std::pair<std::string, std::string>> extractIntegratedAddress(std::string integratedAddress);

BalanceInfo doWeHaveEnoughBalance(uint64_t amount, uint64_t fee, std::shared_ptr<WalletInfo> walletInfo,
                                  uint32_t height, uint64_t nodeFee);
