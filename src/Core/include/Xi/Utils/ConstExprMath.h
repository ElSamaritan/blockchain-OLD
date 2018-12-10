#pragma once

#include <cinttypes>

namespace Xi {
static inline constexpr uint32_t pow(uint32_t base, uint32_t exponent) {
  return exponent == 0 ? 1 : base * pow(base, exponent - 1);
}
}
