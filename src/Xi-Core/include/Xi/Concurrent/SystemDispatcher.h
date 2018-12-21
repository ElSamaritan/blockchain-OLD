#pragma once

#include <System/Dispatcher.h>

#include "Xi/Concurrent/IDispatcher.h"

namespace Xi {
namespace Concurrent {
class SystemDispatcher : public IDispatcher {
 public:
  SystemDispatcher(System::Dispatcher& dispatcher);
  ~SystemDispatcher() override = default;

  void post(std::function<void()> cn) override;

 private:
  System::Dispatcher& m_dispatcher;
};
}  // namespace Concurrent
}  // namespace Xi
