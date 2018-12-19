#pragma once

#include <cinttypes>

namespace Xi {
/*!
 * \brief pow computes the to the power of function (base^exponent)
 * \param base the base of the formula
 * \param exponent the exponent of the formula
 * \return base^exponent, 1 if exponent is 0
 */
static inline constexpr uint32_t pow(uint32_t base, uint32_t exponent) {
  return exponent == 0 ? 1 : base * pow(base, exponent - 1);
}
}  // namespace Xi
