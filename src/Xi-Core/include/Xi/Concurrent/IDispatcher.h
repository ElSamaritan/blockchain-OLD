#pragma once

#include <functional>

namespace Xi {
namespace Concurrent {
/*!
 * \brief The IDispatcher class abstract the implementation of a synchronization schema to queue tasks for a single
 * thread.
 */
class IDispatcher {
 public:
  virtual ~IDispatcher() = default;

  /*!
   * \brief post pushes a new working task to the queue
   * \param cn the task to execute
   */
  virtual void post(std::function<void()> cn) = 0;
};
}  // namespace Concurrent
}  // namespace Xi
