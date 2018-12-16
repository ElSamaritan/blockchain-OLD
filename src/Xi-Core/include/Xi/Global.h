#pragma once

namespace Xi {
template <typename... Ts>
inline void Unreferenced(Ts&&...) { /* */
}
}  // namespace Xi

#define XI_UNUSED(...) \
  { (decltype(Xi::Unreferenced(__VA_ARGS__)))0; }

#define XI_DELETE_COPY(CLASS_NAME)        \
  CLASS_NAME(const CLASS_NAME&) = delete; \
  CLASS_NAME& operator=(const CLASS_NAME&) = delete

#define XI_DEFAULT_COPY(CLASS_NAME)        \
  CLASS_NAME(const CLASS_NAME&) = default; \
  CLASS_NAME& operator=(const CLASS_NAME&) = default

#define XI_DELETE_MOVE(CLASS_NAME)   \
  CLASS_NAME(CLASS_NAME&&) = delete; \
  CLASS_NAME& operator=(CLASS_NAME&&) = delete

#define XI_DEFAULT_MOVE(CLASS_NAME)   \
  CLASS_NAME(CLASS_NAME&&) = default; \
  CLASS_NAME& operator=(CLASS_NAME&&) = default
