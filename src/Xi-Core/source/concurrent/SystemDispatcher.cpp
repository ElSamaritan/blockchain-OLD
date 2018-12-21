#include "Xi/Concurrent/SystemDispatcher.h"

#include <utility>

Xi::Concurrent::SystemDispatcher::SystemDispatcher(System::Dispatcher &dispatcher) : m_dispatcher{dispatcher} {}

void Xi::Concurrent::SystemDispatcher::post(std::function<void()> cn) { m_dispatcher.remoteSpawn(std::move(cn)); }
