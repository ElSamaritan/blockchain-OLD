#pragma once

#include <string>

#include <Xi/Utils/Conversion.h>

namespace CryptoNote {
namespace Config {
namespace Database {
inline std::string blocksFilename() { return "xi-blocks.bin"; }
inline std::string blockIndicesFilename() { return "xi-blockindices.bin"; }
inline std::string pooldataFilename() { return "xi-transaction-pool.bin"; }

inline constexpr uint64_t writeBufferSize() { return 256_MB; }
inline constexpr uint64_t readBufferSize() { return 10_MB; }
inline constexpr uint16_t maximumOpenFiles() { return 100; }
inline constexpr uint16_t backgroundThreads() { return 2; }
}
}
}
