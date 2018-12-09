#pragma once

#include "crypto/hash.h"
#include "crypto/cnx/configuration.h"

namespace Crypto {
namespace CNX {

struct Hash_v0 : Configuration<2048, 2 * 1024, 4 * 1024, 1 * 256 * 1024, 2 * 256 * 1024> {
  void operator()(const void *data, size_t length, Hash &hash, uint32_t height) const;
};

}  // namespace CNX
}  // namespace Crypto
