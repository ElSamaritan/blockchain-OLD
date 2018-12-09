#pragma once

namespace Xi {
template <typename... Ts>
inline void Unreferenced(Ts&&...) { /* */
}
}  // namespace Xi

#define XI_UNUSED(...) \
  { (decltype(Xi::Unreferenced(__VA_ARGS__)))0; }
