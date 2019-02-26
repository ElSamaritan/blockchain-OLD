/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/Transactions/PendingTransactionInfo.h"

CryptoNote::PendingTransactionInfo::PendingTransactionInfo(CachedTransaction tx, CryptoNote::EligibleIndex index,
                                                           PosixTimestamp time)
    : m_transaction{std::make_shared<CachedTransaction>(std::move(tx))}, m_eligibleIndex{index}, m_receiveTime{time} {}

const CryptoNote::CachedTransaction &CryptoNote::PendingTransactionInfo::transaction() const { return *m_transaction; }

CryptoNote::EligibleIndex CryptoNote::PendingTransactionInfo::eligibleIndex() const { return m_eligibleIndex; }

CryptoNote::PosixTimestamp CryptoNote::PendingTransactionInfo::receiveTime() const { return m_receiveTime; }
