// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <Xi/Global.h>

#include "CryptoNoteCore/CryptoNote.h"
#include "P2pProtocolTypes.h"
#include "P2p/P2pPenalty.h"

namespace CryptoNote {

struct CryptoNoteConnectionContext;

struct IP2pEndpoint {
  virtual ~IP2pEndpoint() = default;

  virtual void relay_notify_to_all(int command, const BinaryArray& data_buff,
                                   const net_connection_id* excludeConnection) = 0;
  virtual bool invoke_notify_to_peer(int command, const BinaryArray& req_buff,
                                     const CryptoNote::CryptoNoteConnectionContext& context) = 0;
  virtual uint64_t get_connections_count() = 0;
  virtual void for_each_connection(std::function<void(CryptoNote::CryptoNoteConnectionContext&, PeerIdType)> f) = 0;
  // can be called from external threads
  virtual void externalRelayNotifyToAll(int command, const BinaryArray& data_buff,
                                        const net_connection_id* excludeConnection) = 0;

  /*!
   * \brief report_failure reports a failure, in a p2p command handling, for a given ip address
   * \param ip The sender ip address causing the failure.
   * \param penality The type of failure to compute the weighting of the failure.
   * \return True if the peers ip was added to the ban list, oterwise false.
   */
  virtual bool report_failure(const uint32_t ip, P2pPenalty penalty) = 0;

  /*!
   * \brief report_success reports a success of a p2p command from a given ip address.
   * \param ip The sender ip address.
   *
   * This command slowly decreases the penalty score.
   */
  virtual void report_success(const uint32_t ip) = 0;

  /*!
   * \brief is_ip_address_blocked Returns true if the given ip address is blocked.
   * \param ip The ip address to check.
   * \return True if the address is blocked, otherwise false.
   */
  virtual bool is_ip_address_blocked(const uint32_t ip) = 0;
};

struct p2p_endpoint_stub : public IP2pEndpoint {
  virtual void relay_notify_to_all(int command, const BinaryArray& data_buff,
                                   const net_connection_id* excludeConnection) override {
    XI_UNUSED(command, data_buff, excludeConnection);
  }
  virtual bool invoke_notify_to_peer(int command, const BinaryArray& req_buff,
                                     const CryptoNote::CryptoNoteConnectionContext& context) override {
    XI_UNUSED(command, req_buff, context);
    return true;
  }
  virtual void for_each_connection(
      std::function<void(CryptoNote::CryptoNoteConnectionContext&, PeerIdType)> f) override {
    XI_UNUSED(f);
  }
  virtual uint64_t get_connections_count() override { return 0; }
  virtual void externalRelayNotifyToAll(int command, const BinaryArray& data_buff,
                                        const net_connection_id* excludeConnection) override {
    XI_UNUSED(command, data_buff, excludeConnection);
  }

  virtual bool report_failure(const uint32_t ip, P2pPenalty penalty) override {
    XI_UNUSED(ip, penalty);
    return false;
  }
  virtual void report_success(const uint32_t ip) override { XI_UNUSED(ip); }

  virtual bool is_ip_address_blocked(const uint32_t ip) override {
    XI_UNUSED(ip);
    return false;
  }
};
}  // namespace CryptoNote
