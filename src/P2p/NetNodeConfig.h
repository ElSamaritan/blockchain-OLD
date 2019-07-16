// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <chrono>

#include <Xi/Config/Network.h>

#include "P2pProtocolTypes.h"

namespace CryptoNote {

class NetNodeConfig {
 public:
  NetNodeConfig();
  bool init(const Xi::Config::Network::Configuration& netConfig, const std::string& coinName,
            const std::string interface, const uint16_t port, const uint16_t externalPort, const bool localIp,
            const bool hidePort, const std::string dataDir, const std::vector<std::string> addPeers,
            const std::vector<std::string> addExclusiveNodes, const std::vector<std::string> addPriorityNodes,
            const std::vector<std::string> addSeedNodes);

  std::string getP2pStateFilename() const;
  std::string getBindIp() const;
  uint16_t getBindPort() const;
  uint16_t getExternalPort() const;
  bool getAllowLocalIp() const;
  std::vector<PeerlistEntry> getPeers() const;
  std::vector<NetworkAddress> getPriorityNodes() const;
  std::vector<NetworkAddress> getExclusiveNodes() const;
  std::vector<NetworkAddress> getSeedNodes() const;
  bool getHideMyPort() const;
  std::string getConfigFolder() const;
  std::chrono::seconds getBlockDuration() const;
  bool getAutoBlock() const;
  const std::string& appIdentifier() const;
  const Xi::Config::Network::Configuration& network() const;

  void setP2pStateFilename(const std::string& filename);
  void setNetwork(Xi::Config::Network::Type network);
  void setBindIp(const std::string& ip);
  void setBindPort(uint16_t port);
  void setExternalPort(uint16_t port);
  void setAllowLocalIp(bool allow);
  void setPeers(const std::vector<PeerlistEntry>& peerList);
  void setPriorityNodes(const std::vector<NetworkAddress>& addresses);
  void setExclusiveNodes(const std::vector<NetworkAddress>& addresses);
  void setSeedNodes(const std::vector<NetworkAddress>& addresses);
  void setHideMyPort(bool hide);
  void setConfigFolder(const std::string& folder);
  void setBlockDuration(std::chrono::seconds duration);
  void setAutoBlock(bool enabled);

 private:
  std::string bindIp;
  uint16_t bindPort;
  uint16_t externalPort;
  bool allowLocalIp;
  std::vector<PeerlistEntry> peers;
  std::vector<NetworkAddress> priorityNodes;
  std::vector<NetworkAddress> exclusiveNodes;
  std::vector<NetworkAddress> seedNodes;
  bool hideMyPort;
  std::string configFolder;
  std::string p2pStateFilename;
  std::chrono::seconds m_blockDuration;
  bool m_autoBlock;
  std::string m_appid;
  Xi::Config::Network::Configuration m_netConfig;
};

}  // namespace CryptoNote
