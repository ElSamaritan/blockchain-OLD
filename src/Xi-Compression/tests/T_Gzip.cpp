#include <gmock/gmock.h>
#include <Xi/Compression/Gzip.h>

#include <random>

#include <Xi/Utils/Conversion.h>

#define XI_TESTSUITE T_Xi_Compression_Gzip

class XI_TESTSUITE : public ::testing::Test {
 public:
  const uint64_t DataSize = 16_kB;
  std::string randomData;

  void SetUp() override {
    std::default_random_engine rnd;
    rnd.seed(0);
    randomData.resize(DataSize, '\0');
    for (uint64_t i = 0u; i < DataSize / sizeof(std::default_random_engine::result_type); ++i)
      reinterpret_cast<std::default_random_engine::result_type*>(&randomData[0])[i] = rnd();
  }
};

TEST_F(XI_TESTSUITE, Compression) {
  using namespace ::testing;
  using namespace ::Xi::Compression;

  EXPECT_ANY_THROW(Gzip::compress(randomData, -2));
  EXPECT_ANY_THROW(Gzip::compress(randomData, 10));

  const auto nonCompressed = Gzip::compress(randomData, 0);
  EXPECT_TRUE(Gzip::isCompressed(nonCompressed));
  const auto lowCompressed = Gzip::compress(randomData, 3);
  EXPECT_TRUE(Gzip::isCompressed(lowCompressed));
  const auto maxCompressed = Gzip::compress(randomData, 9);
  EXPECT_TRUE(Gzip::isCompressed(maxCompressed));
}

TEST_F(XI_TESTSUITE, Decompression) {
  using namespace ::testing;
  using namespace ::Xi::Compression;

  EXPECT_ANY_THROW(Gzip::compress(randomData, -2));
  EXPECT_ANY_THROW(Gzip::compress(randomData, 10));

  const auto compressed = Gzip::compress(randomData);
  EXPECT_TRUE(Gzip::isCompressed(compressed));
  const auto decompressed = Gzip::decompress(compressed);
  EXPECT_FALSE(Gzip::isCompressed(decompressed));

  for (uint64_t i = 0u; i < DataSize; ++i) EXPECT_EQ(decompressed[i], randomData[i]);
}
