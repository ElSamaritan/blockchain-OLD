#include <gmock/gmock.h>
#include <Xi/Utils/Base64.h>

#include <string>

#define XI_TESTSUITE T_Xi_Utils_Base64

namespace {
const std::string DecodedString{"This is a test string for Xi."};
const std::string EncodedString{"VGhpcyBpcyBhIHRlc3Qgc3RyaW5nIGZvciBYaS4="};
}  // namespace

TEST(XI_TESTSUITE, Encode) {
  using namespace ::testing;

  EXPECT_THAT(Xi::Base64::encode(DecodedString), Eq(EncodedString));
}

TEST(XI_TESTSUITE, Decode) {
  using namespace ::testing;

  EXPECT_THAT(Xi::Base64::decode(EncodedString), Eq(DecodedString));
}
