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

void transfer(std::shared_ptr<WalletInfo> walletInfo, bool sendAll, CryptoNote::INode& node);

void doTransfer(std::string address, uint64_t amount, uint64_t fee, std::string extra,
                std::shared_ptr<WalletInfo> walletInfo, bool integratedAddress, uint64_t mixin,
                std::optional<CryptoNote::FeeAddress> nodeFees, std::string originalAddress, uint64_t unlockTimestamp,
                const CryptoNote::Currency& currency);

void splitTX(CryptoNote::WalletGreen& wallet, const CryptoNote::TransactionParameters splitTXParams, uint64_t nodeFee);

void sendTX(std::shared_ptr<WalletInfo> walletInfo, CryptoNote::TransactionParameters p, uint64_t nodeFee = 0);

bool confirmTransaction(CryptoNote::TransactionParameters t, std::shared_ptr<WalletInfo> walletInfo,
                        bool integratedAddress, uint64_t nodeFee, std::string originalAddress);

bool parseAmount(std::string amountString, const CryptoNote::Currency& currency);

bool parseStandardAddress(std::string address, const CryptoNote::Currency& currency, bool printErrors = false);

bool parseIntegratedAddress(std::string address, const CryptoNote::Currency& currency);

bool parseFee(std::string feeString, const uint64_t minFee, const CryptoNote::Currency& currency);

void handleTransferError(const std::system_error& e);

AddressType parseAddress(std::string address, const CryptoNote::Currency& currency);

std::string getExtraFromPaymentID(std::string paymentID);

Maybe<std::string> getPaymentID(std::string msg);

Maybe<std::string> getExtra();

Maybe<std::pair<AddressType, std::string>> getAddress(std::string msg, const CryptoNote::Currency& currency);

Maybe<uint64_t> getFee(const uint64_t minFee, const CryptoNote::Currency& currency);

Maybe<uint64_t> getTransferAmount(const CryptoNote::Currency& currency);

Maybe<uint64_t> getTransferUnlock(CryptoNote::BlockVersion version, const CryptoNote::Currency& currency);

bool parseTransferUnlock(const std::string& str, const CryptoNote::BlockVersion version,
                         const CryptoNote::Currency& currency, uint64_t& out);

Maybe<std::pair<std::string, std::string>> extractIntegratedAddress(std::string integratedAddress,
                                                                    const CryptoNote::Currency& currency);

BalanceInfo doWeHaveEnoughBalance(uint64_t amount, uint64_t fee, std::shared_ptr<WalletInfo> walletInfo,
                                  uint64_t nodeFee);
