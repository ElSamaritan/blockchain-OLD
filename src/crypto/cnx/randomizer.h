﻿#pragma once

#include <vector>

#include "crypto/hash.h"

namespace Crypto {
namespace CNX {

struct Randomizer {
  Randomizer(uint32_t maxSize);
  ~Randomizer();

  CN_ADAPTIVE_RandomValues Handle;

  void reset(uint32_t size);

  uint32_t size() const;

private:
  std::vector<uint8_t> _Operators;
  std::vector<uint32_t> _Indices;
  std::vector<int8_t> _Values;
};

}
}
