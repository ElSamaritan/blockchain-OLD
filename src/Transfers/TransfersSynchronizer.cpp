// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The BBSCoin Developers
// Copyright (c) 2018, The Karbo Developers
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include "TransfersSynchronizer.h"
#include "TransfersConsumer.h"

#include <Xi/Exceptions.hpp>

#include "Common/StdInputStream.h"
#include "Common/StdOutputStream.h"
#include "CryptoNoteCore/CryptoNoteBasicImpl.h"
#include "Serialization/BinaryInputStreamSerializer.h"
#include "Serialization/BinaryOutputStreamSerializer.h"

using namespace Common;
using namespace Crypto;

namespace CryptoNote {

const uint32_t TRANSFERS_STORAGE_ARCHIVE_VERSION = 0;

TransfersSyncronizer::TransfersSyncronizer(const CryptoNote::Currency& currency, Logging::ILogger& logger,
                                           IBlockchainSynchronizer& sync, INode& node)
    : m_currency(currency), m_logger(logger, "TransfersSyncronizer"), m_sync(sync), m_node(node) {}

TransfersSyncronizer::~TransfersSyncronizer() {
  m_sync.stop();
  for (const auto& kv : m_consumers) {
    m_sync.removeConsumer(kv.second.get());
  }
}

void TransfersSyncronizer::initTransactionPool(const std::unordered_set<Crypto::Hash>& uncommitedTransactions) {
  for (auto it = m_consumers.begin(); it != m_consumers.end(); ++it) {
    it->second->initTransactionPool(uncommitedTransactions);
  }
}

ITransfersSubscription& TransfersSyncronizer::addSubscription(const AccountSubscription& acc) {
  auto it = m_consumers.find(acc.keys.address.viewPublicKey);

  if (it == m_consumers.end()) {
    std::unique_ptr<TransfersConsumer> consumer(
        new TransfersConsumer(m_currency, m_node, m_logger.getLogger(), acc.keys.viewSecretKey));

    m_sync.addConsumer(consumer.get());
    consumer->addObserver(this);
    it = m_consumers.insert(std::make_pair(acc.keys.address.viewPublicKey, std::move(consumer))).first;
  }

  return it->second->addSubscription(acc);
}

bool TransfersSyncronizer::removeSubscription(const AccountPublicAddress& acc) {
  auto it = m_consumers.find(acc.viewPublicKey);
  if (it == m_consumers.end()) return false;

  if (it->second->removeSubscription(acc)) {
    m_sync.removeConsumer(it->second.get());
    m_consumers.erase(it);

    m_subscribers.erase(acc.viewPublicKey);
  }

  return true;
}

void TransfersSyncronizer::getSubscriptions(std::vector<AccountPublicAddress>& subscriptions) {
  for (const auto& kv : m_consumers) {
    kv.second->getSubscriptions(subscriptions);
  }
}

ITransfersSubscription* TransfersSyncronizer::getSubscription(const AccountPublicAddress& acc) {
  auto it = m_consumers.find(acc.viewPublicKey);
  return (it == m_consumers.end()) ? nullptr : it->second->getSubscription(acc);
}

void TransfersSyncronizer::addPublicKeysSeen(const AccountPublicAddress& acc, const Crypto::Hash& transactionHash,
                                             const Crypto::PublicKey& outputKey) {
  auto it = m_consumers.find(acc.viewPublicKey);
  if (it != m_consumers.end()) {
    it->second->addPublicKeysSeen(transactionHash, outputKey);
  }
}

std::vector<Crypto::Hash> TransfersSyncronizer::getViewKeyKnownBlocks(const Crypto::PublicKey& publicViewKey) {
  auto it = m_consumers.find(publicViewKey);
  if (it == m_consumers.end()) {
    throw std::invalid_argument("Consumer not found");
  }

  return m_sync.getConsumerKnownBlocks(*it->second);
}

void TransfersSyncronizer::onBlocksAdded(IBlockchainConsumer* consumer, const std::vector<Crypto::Hash>& blockHashes) {
  auto it = findSubscriberForConsumer(consumer);
  if (it != m_subscribers.end()) {
    it->second->notify(&ITransfersSynchronizerObserver::onBlocksAdded, it->first, blockHashes);
  }
}

void TransfersSyncronizer::onBlockchainDetach(IBlockchainConsumer* consumer, BlockHeight blockHeight) {
  auto it = findSubscriberForConsumer(consumer);
  if (it != m_subscribers.end()) {
    it->second->notify(&ITransfersSynchronizerObserver::onBlockchainDetach, it->first, blockHeight);
  }
}

void TransfersSyncronizer::onTransactionDeleteBegin(IBlockchainConsumer* consumer, Crypto::Hash transactionHash) {
  auto it = findSubscriberForConsumer(consumer);
  if (it != m_subscribers.end()) {
    it->second->notify(&ITransfersSynchronizerObserver::onTransactionDeleteBegin, it->first, transactionHash);
  }
}

void TransfersSyncronizer::onTransactionDeleteEnd(IBlockchainConsumer* consumer, Crypto::Hash transactionHash) {
  auto it = findSubscriberForConsumer(consumer);
  if (it != m_subscribers.end()) {
    it->second->notify(&ITransfersSynchronizerObserver::onTransactionDeleteEnd, it->first, transactionHash);
  }
}

void TransfersSyncronizer::onTransactionUpdated(IBlockchainConsumer* consumer, const Crypto::Hash& transactionHash,
                                                const std::vector<ITransfersContainer*>& containers) {
  auto it = findSubscriberForConsumer(consumer);
  if (it != m_subscribers.end()) {
    it->second->notify(&ITransfersSynchronizerObserver::onTransactionUpdated, it->first, transactionHash, containers);
  }
}

void TransfersSyncronizer::subscribeConsumerNotifications(const Crypto::PublicKey& viewPublicKey,
                                                          ITransfersSynchronizerObserver* observer) {
  auto it = m_subscribers.find(viewPublicKey);
  if (it != m_subscribers.end()) {
    it->second->add(observer);
    return;
  }

  auto insertedIt =
      m_subscribers.emplace(viewPublicKey, std::unique_ptr<SubscribersNotifier>(new SubscribersNotifier())).first;
  insertedIt->second->add(observer);
}

void TransfersSyncronizer::unsubscribeConsumerNotifications(const Crypto::PublicKey& viewPublicKey,
                                                            ITransfersSynchronizerObserver* observer) {
  m_subscribers.at(viewPublicKey)->remove(observer);
}

bool TransfersSyncronizer::save(std::ostream& os) {
  XI_RETURN_EC_IF_NOT(m_sync.save(os), false);

  StdOutputStream stream(os);
  CryptoNote::BinaryOutputStreamSerializer s(stream);
  XI_RETURN_EC_IF_NOT(s(const_cast<uint32_t&>(TRANSFERS_STORAGE_ARCHIVE_VERSION), "version"), false);

  size_t subscriptionCount = m_consumers.size();

  XI_RETURN_EC_IF_NOT(s.beginArray(subscriptionCount, "consumers"), false);

  for (const auto& consumer : m_consumers) {
    XI_RETURN_EC_IF_NOT(s.beginObject(""), false);
    XI_RETURN_EC_IF_NOT(s(const_cast<PublicKey&>(consumer.first), "view_key"), false);

    std::stringstream consumerState;
    // synchronization state
    XI_RETURN_EC_IF_NOT(m_sync.getConsumerState(consumer.second.get())->save(consumerState), false);

    std::string blob = consumerState.str();
    XI_RETURN_EC_IF_NOT(s(blob, "state"), false);

    std::vector<AccountPublicAddress> subscriptions;
    consumer.second->getSubscriptions(subscriptions);
    size_t subCount = subscriptions.size();

    XI_RETURN_EC_IF_NOT(s.beginArray(subCount, "subscriptions"), false);

    for (auto& addr : subscriptions) {
      auto sub = consumer.second->getSubscription(addr);
      if (sub != nullptr) {
        XI_RETURN_EC_IF_NOT(s.beginObject(""), false);

        std::stringstream subState;
        assert(sub);
        XI_RETURN_EC_IF_NOT(sub->getContainer().save(subState), false);
        // store data block
        std::string innerBlob = subState.str();
        XI_RETURN_EC_IF_NOT(s(addr, "address"), false);
        XI_RETURN_EC_IF_NOT(s(innerBlob, "state"), false);

        XI_RETURN_EC_IF_NOT(s.endObject(), false);
      }
    }

    XI_RETURN_EC_IF_NOT(s.endArray(), false);
    XI_RETURN_EC_IF_NOT(s.endObject(), false);
  }
  return true;
}

namespace {
std::string getObjectState(IStreamSerializable& obj) {
  std::stringstream stream;
  Xi::exceptional_if_not<Xi::RuntimeError>(obj.save(stream), "getObjectState serialization failed");
  return stream.str();
}

void setObjectState(IStreamSerializable& obj, const std::string& state) {
  std::stringstream stream(state);
  Xi::exceptional_if_not<Xi::RuntimeError>(obj.load(stream), "setObjectState deserialization failed");
}

}  // namespace

bool TransfersSyncronizer::load(std::istream& is) {
  XI_RETURN_EC_IF_NOT(m_sync.load(is), false);

  StdInputStream inputStream(is);
  CryptoNote::BinaryInputStreamSerializer s(inputStream);
  uint32_t version = 0;

  XI_RETURN_EC_IF_NOT(s(version, "version"), false);
  XI_RETURN_EC_IF(version > TRANSFERS_STORAGE_ARCHIVE_VERSION, false);

  struct ConsumerState {
    PublicKey viewKey;
    std::string state;
    std::vector<std::pair<AccountPublicAddress, std::string>> subscriptionStates;
  };

  std::vector<ConsumerState> updatedStates;

  try {
    size_t subscriptionCount = 0;
    XI_RETURN_EC_IF_NOT(s.beginArray(subscriptionCount, "consumers"), false);

    while (subscriptionCount--) {
      XI_RETURN_EC_IF_NOT(s.beginObject(""), false);
      PublicKey viewKey;
      XI_RETURN_EC_IF_NOT(s(viewKey, "view_key"), false);

      std::string blob;
      XI_RETURN_EC_IF_NOT(s(blob, "state"), false);

      auto subIter = m_consumers.find(viewKey);
      if (subIter != m_consumers.end()) {
        auto consumerState = m_sync.getConsumerState(subIter->second.get());
        assert(consumerState);

        {
          // store previous state
          auto prevConsumerState = getObjectState(*consumerState);
          // load consumer state
          setObjectState(*consumerState, blob);
          updatedStates.push_back(ConsumerState{viewKey, std::move(prevConsumerState), {}});
        }

        // load subscriptions
        size_t subCount = 0;
        XI_RETURN_EC_IF_NOT(s.beginArray(subCount, "subscriptions"), false);

        while (subCount--) {
          XI_RETURN_EC_IF_NOT(s.beginObject(""), false);

          AccountPublicAddress acc;
          std::string state;

          XI_RETURN_EC_IF_NOT(s(acc, "address"), false);
          XI_RETURN_EC_IF_NOT(s(state, "state"), false);

          auto sub = subIter->second->getSubscription(acc);

          if (sub != nullptr) {
            auto prevState = getObjectState(sub->getContainer());
            setObjectState(sub->getContainer(), state);
            updatedStates.back().subscriptionStates.push_back(std::make_pair(acc, prevState));
          } else {
            m_logger(Logging::DEBUGGING) << "Subscription not found: " << m_currency.accountAddressAsString(acc);
          }

          XI_RETURN_EC_IF_NOT(s.endObject(), false);
        }

        XI_RETURN_EC_IF_NOT(s.endArray(), false);
      } else {
        m_logger(Logging::DEBUGGING) << "Consumer not found: " << viewKey;
      }

      XI_RETURN_EC_IF_NOT(s.endObject(), false);
    }

    XI_RETURN_EC_IF_NOT(s.endArray(), false);

    return true;
  } catch (...) {
    // rollback state
    for (const auto& consumerState : updatedStates) {
      auto consumer = m_consumers.find(consumerState.viewKey)->second.get();
      setObjectState(*m_sync.getConsumerState(consumer), consumerState.state);
      for (const auto& sub : consumerState.subscriptionStates) {
        setObjectState(consumer->getSubscription(sub.first)->getContainer(), sub.second);
      }
    }
    throw;
  }
}

bool TransfersSyncronizer::findViewKeyForConsumer(IBlockchainConsumer* consumer, Crypto::PublicKey& viewKey) const {
  // since we have only couple of consumers linear complexity is fine
  auto it = std::find_if(
      m_consumers.begin(), m_consumers.end(),
      [consumer](const ConsumersContainer::value_type& subscription) { return subscription.second.get() == consumer; });

  if (it == m_consumers.end()) {
    return false;
  }

  viewKey = it->first;
  return true;
}

TransfersSyncronizer::SubscribersContainer::const_iterator TransfersSyncronizer::findSubscriberForConsumer(
    IBlockchainConsumer* consumer) const {
  Crypto::PublicKey viewKey;
  if (findViewKeyForConsumer(consumer, viewKey)) {
    auto it = m_subscribers.find(viewKey);
    if (it != m_subscribers.end()) {
      return it;
    }
  }

  return m_subscribers.end();
}

}  // namespace CryptoNote
