#pragma once

#include <functional>

namespace Xi {
namespace Concurrent {
class IDispatcher {
 public:
  virtual ~IDispatcher() = default;

  virtual void post(std::function<void()> cn) = 0;
};
}  // namespace Concurrent
}  // namespace Xi
