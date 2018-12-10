#pragma once

#include <Xi/Utils/ConstExprMath.h>

namespace CryptoNote {
namespace Config {
namespace Coin {
constexpr uint8_t numberOfDecimalPoints() { return 6; }
constexpr uint64_t toAtomicUnits(uint64_t coins) { return Xi::pow(10, 6) * coins; }
constexpr uint64_t totalSupply() { return toAtomicUnits(55000000); }
}  // namespace Coin
}
}
