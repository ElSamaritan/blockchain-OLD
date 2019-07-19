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

#if defined(_MSC_VER)
#define _DISABLE_EXTENDED_ALIGNED_STORAGE 1
#endif

#include "Xi/ProofOfWork/Cnx.hpp"

#include <memory>
#include <vector>

#include <Xi/Crypto/Hash/Keccak.hh>
#include <Xi/Crypto/Hash/Sha2.hh>

#include "groestl/groestl.h"
#include "blake/blake256.h"
#include "jh/jh.h"

#include <iostream>

namespace Xi {
namespace ProofOfWork {

namespace Cnx {

class INode {
 protected:
  mutable bool m_hasValue = false;
  mutable ::Crypto::Hash m_current;

 public:
  virtual ~INode() = default;

 protected:
  virtual void doUpdate(ConstByteSpan blob) = 0;
  virtual void doFinal(::Crypto::Hash& out) const = 0;
  virtual std::shared_ptr<INode> doCopy() const = 0;

 public:
  void update(ConstByteSpan blob) {
    doUpdate(blob);
    m_hasValue = false;
  }

  const ::Crypto::Hash& hash() const {
    if (!m_hasValue) {
      doFinal(m_current);
      m_hasValue = true;
    }
    return m_current;
  }

  std::shared_ptr<INode> copy() const {
    return doCopy();
  }
};

class KeccakNode final : public INode {
 private:
  xi_crypto_hash_keccak_state m_state;

 public:
  ~KeccakNode() override = default;
  explicit KeccakNode(const xi_crypto_hash_keccak_state& state) {
    std::memcpy(std::addressof(m_state), std::addressof(state), sizeof(xi_crypto_hash_keccak_state));
  }
  explicit KeccakNode() {
    xi_crypto_hash_keccak_init(std::addressof(m_state));
  }

  void doUpdate(ConstByteSpan blob) override {
    xi_crypto_hash_keccak_update(std::addressof(m_state), blob.data(), blob.size_bytes());
  }

  std::shared_ptr<INode> doCopy() const override {
    return std::make_shared<KeccakNode>(m_state);
  }

  void doFinal(::Crypto::Hash& out) const override {
    xi_crypto_hash_keccak_state state{};
    std::memcpy(std::addressof(state), std::addressof(m_state), sizeof(xi_crypto_hash_keccak_state));
    xi_crypto_hash_keccak_finish(std::addressof(state), out.data());
  }
};

class GroestlNode final : public INode {
 private:
  groestl_hash_state m_state;

 public:
  ~GroestlNode() override = default;
  explicit GroestlNode(const groestl_hash_state& state) {
    std::memcpy(std::addressof(m_state), std::addressof(state), sizeof(groestl_hash_state));
  }
  GroestlNode() {
    groestl_init(std::addressof(m_state));
  }

  void doUpdate(ConstByteSpan blob) override {
    groestl_update(std::addressof(m_state), blob.data(), blob.size_bytes());
  }

  std::shared_ptr<INode> doCopy() const override {
    return std::make_shared<GroestlNode>(m_state);
  }

  void doFinal(::Crypto::Hash& out) const override {
    groestl_hash_state state{};
    std::memcpy(std::addressof(state), std::addressof(m_state), sizeof(groestl_hash_state));
    groestl_final(std::addressof(state), out.data());
  }
};

class BlakeNode final : public INode {
 private:
  blake_state m_state;

 public:
  ~BlakeNode() override = default;
  explicit BlakeNode(const blake_state& state) {
    std::memcpy(std::addressof(m_state), std::addressof(state), sizeof(blake_state));
  }
  BlakeNode() {
    blake224_init(std::addressof(m_state));
  }

  void doUpdate(ConstByteSpan blob) override {
    blake224_update(std::addressof(m_state), blob.data(), blob.size_bytes());
  }

  std::shared_ptr<INode> doCopy() const override {
    return std::make_shared<BlakeNode>(m_state);
  }

  void doFinal(::Crypto::Hash& out) const override {
    blake_state state{};
    std::memcpy(std::addressof(state), std::addressof(m_state), sizeof(blake_state));
    blake256_final(std::addressof(state), out.data());
  }
};

class JhNode final : public INode {
 private:
  jh_hash_state m_state;

 public:
  ~JhNode() override = default;
  explicit JhNode(const jh_hash_state& state) {
    std::memcpy(std::addressof(m_state), std::addressof(state), sizeof(jh_hash_state));
  }
  JhNode() {
    jh_init(std::addressof(m_state), static_cast<int>(::Crypto::Hash::bytes()));
  }

  void doUpdate(ConstByteSpan blob) override {
    jh_update(std::addressof(m_state), blob.data(), blob.size_bytes());
  }

  std::shared_ptr<INode> doCopy() const override {
    return std::make_shared<JhNode>(m_state);
  }

  void doFinal(::Crypto::Hash& out) const override {
    jh_hash_state state{};
    std::memcpy(std::addressof(state), std::addressof(m_state), sizeof(jh_hash_state));
    jh_final(std::addressof(state), out.data());
  }
};

struct NodeComparator {
  bool operator()(const std::shared_ptr<INode>& lhs, const std::shared_ptr<INode>& rhs) const {
    return std::memcmp(lhs->hash().data(), rhs->hash().data(), ::Crypto::Hash::bytes()) <= 0;
  }
};

struct ReversePredicate {
  bool operator()(const std::shared_ptr<INode>& node) const {
    return node->hash()[::Crypto::Hash::bytes() - 1] & 0b1;
  }
};

struct RangePredicate {
  size_t operator()(const std::shared_ptr<INode>& node) const {
    return (node->hash()[::Crypto::Hash::bytes() - 1] & 0b11) + 1;
  }
};

struct NodePredicate {
  std::shared_ptr<INode> operator()(const Byte byte) const {
    std::shared_ptr<INode> reval{};
    switch (byte & 0b11) {
      case 0:
        reval = std::make_shared<KeccakNode>();
        break;
      case 1:
        reval = std::make_shared<GroestlNode>();
        break;
      case 2:
        reval = std::make_shared<BlakeNode>();
        break;
      default:
        reval = std::make_shared<JhNode>();
        break;
    }
    return reval;
  }

  std::shared_ptr<INode> operator()(const std::shared_ptr<INode>& node) const {
    std::shared_ptr<INode> reval = (*this)(node->hash()[::Crypto::Hash::bytes() - 1]);
    reval->update(node->hash().span());
    return reval;
  }

  std::shared_ptr<INode> operator()(ConstByteSpan blob) const {
    if (blob.empty()) {
      return std::make_shared<KeccakNode>();
    } else {
      std::shared_ptr<INode> reval = (*this)(blob.data()[0]);
      reval->update(blob);
      return reval;
    }
  }
};

#if defined(_MSC_VER) && _MSC_VER > 19000

#endif

void cnx(ConstByteSpan blob, ::Crypto::Hash& hash, const size_t insertions) {
  const ReversePredicate revpred{};
  const RangePredicate rngpred{};
  const NodePredicate nodepred{};

  using set_t = std::set<std::shared_ptr<INode>, NodeComparator>;
  using set_iterator_t = set_t::iterator;

  set_t hashTree{};
  auto previousInsertion = hashTree.insert(nodepred(blob)).first;

  for (size_t i = 0; i < insertions;) {
    auto iNode = nodepred(*previousInsertion);

    auto insertion = hashTree.insert(iNode);
    while (!insertion.second) {
      iNode->update(iNode->hash().span());
      insertion = hashTree.insert(iNode);
    }
    if (++i >= insertions) {
      break;
    }
    previousInsertion = insertion.first;

    std::vector<std::shared_ptr<INode>> previousNodes{};
    set_iterator_t begin, end;
    const auto range = rngpred(*previousInsertion);

    if (revpred(*previousInsertion)) {
      if (previousInsertion == hashTree.begin()) {
        begin = previousInsertion;
        end = previousInsertion;
      } else {
        end = previousInsertion;
        auto j = begin = std::prev(previousInsertion);
        for (size_t k = 0; k < range; ++k) {
          previousNodes.push_back(*j);
          if (j == hashTree.begin()) {
            break;
          }
          std::advance(j, -1);
        }
        begin = j;
      }
    } else {
      auto l = begin = std::next(previousInsertion);
      for (size_t k = 0; l != hashTree.end() && k < range; ++k) {
        previousNodes.push_back(*l);
        std::advance(l, 1);
      }
      end = l;
    }

    hashTree.erase(begin, end);
    for (auto j = previousNodes.rbegin(); j != previousNodes.rend(); ++j) {
      (*j)->update((*previousInsertion)->hash().span());
      hashTree.insert(*j);
      auto jNode = nodepred(*j);
      insertion = hashTree.insert(jNode);
      while (!insertion.second) {
        jNode->update(jNode->hash().span());
        insertion = hashTree.insert(jNode);
      }
      previousInsertion = insertion.first;
      if (++i >= insertions) {
        break;
      }
    }
  }

  xi_crypto_hash_keccak_state finalState{};
  xi_crypto_hash_keccak_init(std::addressof(finalState));
  for (const auto& iHash : hashTree) {
    xi_crypto_hash_keccak_update(std::addressof(finalState), iHash->hash().data(), ::Crypto::Hash::bytes());
  }
  xi_crypto_hash_keccak_finish(std::addressof(finalState), hash.data());
}

}  // namespace Cnx

void CNX_v1_Light::operator()(ConstByteSpan blob, ::Crypto::Hash& hash) const {
  Cnx::cnx(blob, hash, 64);
}

void CNX_v1::operator()(ConstByteSpan blob, ::Crypto::Hash& hash) const {
  Cnx::cnx(blob, hash, 256);
}

void CNX_v1_Heavy::operator()(ConstByteSpan blob, ::Crypto::Hash& hash) const {
  Cnx::cnx(blob, hash, 1024);
}

}  // namespace ProofOfWork
}  // namespace Xi
