﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "NetNodeConfig.h"

#include <limits>
#include <sstream>

#include <Xi/Config.h>
#include <Xi/Algorithm/String.h>

#include <Xi/Crypto/Random/Random.hh>
#include <Common/Util.h>
#include "Common/CommandLine.h"
#include "Common/StringTools.h"
#include "crypto/crypto.h"

namespace CryptoNote {
namespace {

bool parsePeerFromString(NetworkAddress& pe, const std::string& node_addr) {
  return Common::parseIpAddressAndPort(pe.ip, pe.port, node_addr);
}

bool parsePeersAndAddToNetworkContainer(const std::vector<std::string> peerList,
                                        std::vector<NetworkAddress>& container) {
  for (const std::string& peer : peerList) {
    NetworkAddress networkAddress = NetworkAddress();
    if (!parsePeerFromString(networkAddress, peer)) {
      return false;
    }
    container.push_back(networkAddress);
  }
  return true;
}

bool parsePeersAndAddToPeerListContainer(const std::vector<std::string> peerList,
                                         std::vector<PeerlistEntry>& container) {
  for (const std::string& peer : peerList) {
    PeerlistEntry peerListEntry = PeerlistEntry();
    auto ec = Xi::Crypto::Random::generate(Xi::asByteSpan(&peerListEntry.id, sizeof(PeerIdType)));
    XI_RETURN_EC_IF_NOT(ec == Xi::Crypto::Random::RandomError::Success, false);
    if (!parsePeerFromString(peerListEntry.address, peer)) {
      return false;
    }
    container.push_back(peerListEntry);
  }
  return true;
}

}  // namespace

NetNodeConfig::NetNodeConfig() {
  bindIp = "";
  bindPort = 0;
  externalPort = 0;
  allowLocalIp = false;
  hideMyPort = false;
  configFolder = Tools::getDefaultDataDirectory();
  m_blockDuration = std::chrono::hours{1};
  m_autoBlock = false;
  m_appid = "";
}

bool NetNodeConfig::init(const Xi::Config::Network::Configuration& netConfig, const std::string& coinName,
                         const std::string interface, const uint16_t port, const uint16_t _externalPort,
                         const bool localIp, const bool hidePort, const std::string dataDir,
                         const std::vector<std::string> addPeers, const std::vector<std::string> addExclusiveNodes,
                         const std::vector<std::string> addPriorityNodes, const std::vector<std::string> addSeedNodes) {
  m_netConfig = netConfig;
  bindIp = interface;

  bindPort = port;
  externalPort = _externalPort;
  configFolder = dataDir;

  allowLocalIp = localIp;
  hideMyPort = hidePort;

  m_appid = coinName;
  if (!netConfig.isMainNet()) {
    m_appid += std::string{"."} + toString(netConfig.type());
  }

  p2pStateFilename = Xi::Config::P2P::stateBackupFilename();

  if (!addPeers.empty()) {
    if (!parsePeersAndAddToPeerListContainer(addPeers, peers)) {
      return false;
    }
  }

  if (!addExclusiveNodes.empty()) {
    if (!parsePeersAndAddToNetworkContainer(addExclusiveNodes, exclusiveNodes)) {
      return false;
    }
  }

  if (!addPriorityNodes.empty()) {
    if (!parsePeersAndAddToNetworkContainer(addPriorityNodes, priorityNodes)) {
      return false;
    }
  }

  if (!addSeedNodes.empty()) {
    if (!parsePeersAndAddToNetworkContainer(addSeedNodes, seedNodes)) {
      return false;
    }
  }

  return true;
}

std::string NetNodeConfig::getP2pStateFilename() const {
  return "p2p";
}

std::string NetNodeConfig::getBindIp() const {
  return bindIp;
}

uint16_t NetNodeConfig::getBindPort() const {
  return bindPort;
}

uint16_t NetNodeConfig::getExternalPort() const {
  return externalPort;
}

bool NetNodeConfig::getAllowLocalIp() const {
  return allowLocalIp;
}

std::vector<PeerlistEntry> NetNodeConfig::getPeers() const {
  return peers;
}

std::vector<NetworkAddress> NetNodeConfig::getPriorityNodes() const {
  return priorityNodes;
}

std::vector<NetworkAddress> NetNodeConfig::getExclusiveNodes() const {
  return exclusiveNodes;
}

std::vector<NetworkAddress> NetNodeConfig::getSeedNodes() const {
  return seedNodes;
}

bool NetNodeConfig::getHideMyPort() const {
  return hideMyPort;
}

std::string NetNodeConfig::getConfigFolder() const {
  return configFolder;
}

std::chrono::seconds NetNodeConfig::getBlockDuration() const {
  return m_blockDuration;
}

bool NetNodeConfig::getAutoBlock() const {
  return m_autoBlock;
}

const std::string& NetNodeConfig::appIdentifier() const {
  return m_appid;
}

const Xi::Config::Network::Configuration& NetNodeConfig::network() const {
  return m_netConfig;
}

void NetNodeConfig::setP2pStateFilename(const std::string& filename) {
  p2pStateFilename = filename;
}

void NetNodeConfig::setBindIp(const std::string& ip) {
  bindIp = ip;
}

void NetNodeConfig::setBindPort(uint16_t port) {
  bindPort = port;
}

void NetNodeConfig::setExternalPort(uint16_t port) {
  externalPort = port;
}

void NetNodeConfig::setAllowLocalIp(bool allow) {
  allowLocalIp = allow;
}

void NetNodeConfig::setPeers(const std::vector<PeerlistEntry>& peerList) {
  peers = peerList;
}

void NetNodeConfig::setPriorityNodes(const std::vector<NetworkAddress>& addresses) {
  priorityNodes = addresses;
}

void NetNodeConfig::setExclusiveNodes(const std::vector<NetworkAddress>& addresses) {
  exclusiveNodes = addresses;
}

void NetNodeConfig::setSeedNodes(const std::vector<NetworkAddress>& addresses) {
  seedNodes = addresses;
}

void NetNodeConfig::setHideMyPort(bool hide) {
  hideMyPort = hide;
}

void NetNodeConfig::setConfigFolder(const std::string& folder) {
  configFolder = folder;
}

void NetNodeConfig::setBlockDuration(std::chrono::seconds duration) {
  m_blockDuration = duration;
}

void NetNodeConfig::setAutoBlock(bool enabled) {
  m_autoBlock = enabled;
}

}  // namespace CryptoNote
