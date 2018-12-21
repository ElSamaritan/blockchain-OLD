#pragma once

#include <System/Dispatcher.h>

#include "Xi/Concurrent/IDispatcher.h"

namespace Xi {
namespace Concurrent {
/*!
 * \brief The SystemDispatcher class implements a thread working queue based on the system library dispatcher
 * implementation
 */
class SystemDispatcher : public IDispatcher {
 public:
  /*!
   * \brief SystemDispatcher wraps a system library dispatcher
   * \param dispatcher the system library dispatcher to wrap
   */
  SystemDispatcher(System::Dispatcher& dispatcher);
  ~SystemDispatcher() override = default;

  /*!
   * \brief post enqueues the task to the system library dispatcher
   * \param cn the task to execute
   */
  void post(std::function<void()> cn) override;

 private:
  System::Dispatcher& m_dispatcher;
};
}  // namespace Concurrent
}  // namespace Xi
