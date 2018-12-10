#pragma once

#include <cinttypes>

static inline constexpr uint64_t operator "" _Bytes(unsigned long long bytes){
  return bytes;
}

static inline constexpr uint64_t operator "" _kB(unsigned long long kiloBytes){
  return kiloBytes * 1024;
}

static inline constexpr uint64_t operator "" _MB(unsigned long long megaBytes){
  return megaBytes * 1024 * 1024;
}
