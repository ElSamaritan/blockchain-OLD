#pragma once

#include <cinttypes>
#include <chrono>

#include <Xi/Utils/ConstExprMath.h>

static inline constexpr uint64_t operator"" _Bytes(unsigned long long bytes) { return bytes; }

static inline constexpr uint64_t operator"" _kB(unsigned long long kiloBytes) { return kiloBytes * 1024; }

static inline constexpr uint64_t operator"" _MB(unsigned long long megaBytes) { return megaBytes * 1024 * 1024; }

static inline constexpr std::chrono::milliseconds operator"" _ms(unsigned long long milliseconds) {
  return std::chrono::milliseconds{milliseconds};
}

static inline constexpr std::chrono::seconds operator"" _s(unsigned long long seconds) {
  return std::chrono::seconds{seconds};
}

static inline constexpr std::chrono::minutes operator"" _m(unsigned long long minutes) {
  return std::chrono::minutes{minutes};
}

static inline constexpr std::chrono::hours operator"" _h(unsigned long long hours) { return std::chrono::hours{hours}; }

static inline constexpr std::chrono::hours operator"" _d(unsigned long long days) {
  return std::chrono::hours{days * 24};
}

static inline constexpr uint64_t operator"" _k(unsigned long long kilo) { return kilo * Xi::pow(10, 3); }

static inline constexpr uint64_t operator"" _M(unsigned long long mega) { return mega * Xi::pow(10, 6); }

static inline constexpr uint64_t operator"" _T(unsigned long long giga) { return giga * Xi::pow(10, 9); }

static inline constexpr uint64_t operator"" _G(unsigned long long tera) { return tera * Xi::pow(10, 12); }
