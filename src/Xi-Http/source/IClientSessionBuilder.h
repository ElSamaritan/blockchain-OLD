#pragma once

#include <memory>

namespace Xi {
namespace Http {
class ClientSession;

/*!
 * \brief The IClientSessionBuilder class abstract the way new client sessions are builded.
 */
class IClientSessionBuilder {
 public:
  IClientSessionBuilder() = default;
  virtual ~IClientSessionBuilder() = default;

  virtual std::shared_ptr<ClientSession> makeHttpSession() = 0;
  virtual std::shared_ptr<ClientSession> makeHttpsSession() = 0;
};

}  // namespace Http
}  // namespace Xi
