/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
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

#include "CryptoNoteCore/Transactions/BlockTransactionValidator.h"

CryptoNote::BlockTransactionValidator::BlockTransactionValidator(const CryptoNote::BlockInfo &info,
                                                                 const Checkpoints &checkpoints,
                                                                 const IBlockchainCache &chain,
                                                                 const Currency &currency)
    : TransactionValidator(info.Header.version, chain, currency), m_block{info}, m_checkpoints{checkpoints} {
}

bool CryptoNote::BlockTransactionValidator::checkIfAnySpent(const Crypto::KeyImageSet &keyImages) const {
  return chain().checkIfAnySpent(keyImages, m_block.previousBlockIndex);
}

void CryptoNote::BlockTransactionValidator::fillContext(CryptoNote::TransferValidationContext &context) const {
  context.blockVersion = m_block.Header.version;
  context.previousBlockIndex = m_block.previousBlockIndex;
  context.timestamp = m_block.Header.timestamp;
}
