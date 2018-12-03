#include "crypto/cnx/cnx.h"

#include <vector>
#include <memory>
#include <random>

#include "crypto/keccak.h"

#include "crypto/cnx/randomizer.h"
#include "crypto/cnx/mersenne-twister.h"

namespace {

void (*const extra_hashes[4])(const void *, size_t, char *) = {
    Crypto::hash_extra_blake, Crypto::hash_extra_groestl, Crypto::hash_extra_jh, Crypto::hash_extra_skein
};

/*!
 * \attention Randomizer::size must be a multiple of 32
 * \attention Length must be at least 32 byte.
 */
void init_randomizer(const void *data, size_t length, Crypto::CN_ADAPTIVE_RandomValues* randomizer, Crypto::Hash &hash, Crypto::CNX::MersenneTwister& rnd)
{
  using namespace Crypto;

  static thread_local std::vector<uint8_t> __Data;
  if(__Data.size() != length) __Data.resize(length);
  std::memcpy(__Data.data(), data, length);

  char* hashPtr = reinterpret_cast<char*>(&hash);

  // We assume the hash has been already initialized with some values from the caller, as well as the seed, for initial run.
  for(uint32_t i = 0; i < randomizer->size / 32; ++i) {
    const uint32_t offset = i * 32;

    // First: Pick some random operatos using keccak1600, note since we are using __Data they depend on the previous iteration
    keccak(__Data.data(), static_cast<int>(length), randomizer->operators + offset, 32);
    // Second: Make the values of the randomizer dependent on the last iterations hashPtr, calculated lastly. For the first loop
    // we simply have chosen cn_fast_hash.
    std::memcpy(randomizer->values + offset, hashPtr, 32);

    // Third: In order to make every iteration dependent on the previous we will update our basic __Data block with the operator
    // we just calculated.
    __Data[0] = reinterpret_cast<uint8_t*>(randomizer->values)[0];
    for(uint32_t j = 1; j < length; ++j) {
      cn_adaptive_apply_operator(&__Data[j], randomizer->values + ((offset + j) % randomizer->size),
                                 reinterpret_cast<uint8_t*>(randomizer->values) + offset + ((j - 1) % randomizer->size), 1);
    }

    // Fourth: We need to initialize the indices, we will not bother for now whether they encode legit addresses, the cn_adaptive
    // algorithm will take care of this within the implementation itself.
    // Keccak will ensure we have calculated the linear dependent hash-chain previsouly in order to conitnue.
//    keccak(__Data.data(), static_cast<int>(length), reinterpret_cast<uint8_t*>(hashPtr), 32);
//    for(uint32_t j = 0; j < 32; ++j) {
//      std::memcpy(randomizer->indices + offset + j, hashPtr + j, 1);
//      if(j > 0) {
//        cn_adaptive_apply_operator(reinterpret_cast<uint8_t*>(randomizer->indices) + offset + j,
//                                   reinterpret_cast<int8_t*>(randomizer->indices) + offset + j - 1,
//                                   reinterpret_cast<uint8_t*>(hashPtr) + j, 1);
//      }
//    }

    // Last: We update our hashPtr using the updated __Data block, since we dont want someone to know which algorithm we pick
    // , next we update the seed depening on the modified hash.
//    extra_hashes[rnd.next(4)](__Data.data(), length, hashPtr);
//    rnd.set_seed(reinterpret_cast<uint32_t*>(hashPtr)[rnd.next(8)]);
  }
}

void init_salt(const void *data, size_t length, char* salt, size_t saltLength, Crypto::Hash &hash,
               Crypto::CN_ADAPTIVE_RandomValues* randomizer, Crypto::CNX::MersenneTwister& rnd) {
  using namespace Crypto;

  char* hashPtr = reinterpret_cast<char*>(&hash);
  // Make sure we depedent on the last init_randomizer step
  cn_fast_hash(data, length, hashPtr);
  salt[0] = hashPtr[rnd.next(32)];
  salt[2] = hashPtr[rnd.next(32)];
  for(size_t i = 2; i < saltLength; ++i) {
    const uint32_t iR = i % randomizer->size;
    salt[i] = randomizer->values[iR];
    // In order to make every byte very depedent to be evaulated in the sequence given we will alter the randomizer using the
    // previous byte itself, somehow like the scratchpad alters itself.
    cn_adaptive_apply_operator(reinterpret_cast<uint8_t*>(salt) + i, reinterpret_cast<int8_t*>(salt) + i - 1,
                               reinterpret_cast<uint8_t*>(salt) + i - 2, 1);
  }
}

}

void Crypto::CNX::Hash_v0::operator()(const void *data, size_t length, Crypto::Hash &hash, uint32_t height) const
{
  static thread_local std::unique_ptr<MersenneTwister> __Twister = std::make_unique<MersenneTwister>(0);
  static thread_local std::unique_ptr<Randomizer> __Randomizer = std::make_unique<Randomizer>(1 << 16);
  static thread_local std::vector<char> __Salt;
  static thread_local const uint8_t __Lookup[3] { 2, 4, 8 };

  cn_fast_hash(data, length, reinterpret_cast<char*>(&hash));
  __Twister->set_seed(*reinterpret_cast<uint32_t*>(&hash));

  const uint32_t offset = height % (windowSize());
  const uint32_t scratchpadSize = minScratchpadSize() + offset * slopeScratchpadSize();
  const uint32_t randomizerSize = maxRandomizerSize() - offset * slopeRandomizerSize();
  __Randomizer->reset(randomizerSize);
 init_randomizer(data, length, &__Randomizer->Handle, hash, *__Twister.get());

  // We dont have a wrapper for the salt so we plain
  __Salt.resize(__Randomizer->size(), 0);
  std::memset(__Salt.data(), 0, __Salt.size());
  init_salt(data, length, __Salt.data(), __Randomizer->size(), hash, &__Randomizer->Handle, *__Twister.get());

  auto xx = scratchpadSize / 2;
  auto yy = static_cast<uint16_t>(__Twister->next(1, 2));
  auto zz = static_cast<uint16_t>(__Twister->next(1, 2));
  auto ww = static_cast<uint16_t>(__Twister->next(1, 10000));
  auto lookupIndex = __Twister->next(sizeof (__Lookup));
  cn_adaptive_slow_hash(data, length, reinterpret_cast<char*>(&hash), variant(), false, scratchpadSize / 2,
                        ((height + 1) % 64), &__Randomizer->Handle, __Salt.data(), __Lookup[lookupIndex],
                        static_cast<uint16_t>(xx), yy, zz, ww, scratchpadSize);
}
