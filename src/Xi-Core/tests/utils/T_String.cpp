#include <gmock/gmock.h>
#include <Xi/Utils/String.h>

#include <string>

#define XI_TESTSUITE T_Xi_Utils_String

TEST(XI_TESTSUITE, StartsWith) {
  using namespace ::testing;

  EXPECT_TRUE(Xi::starts_with("", ""));
  EXPECT_TRUE(Xi::starts_with("Xi Blockchain", "Xi"));
  EXPECT_TRUE(Xi::starts_with("Xi Blockchain", ""));
  EXPECT_FALSE(Xi::starts_with("", "Xi"));
  EXPECT_FALSE(Xi::starts_with("Xi Blockchain", "xi"));
  EXPECT_FALSE(Xi::starts_with("Xi Blockchain", "XiB"));
  EXPECT_FALSE(Xi::starts_with("Xi Blockchain", "Xi b"));
}

TEST(XI_TESTSUITE, ToString) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_THAT(to_string(0.257), Eq(std::to_string(0.257)));
  EXPECT_THAT(to_string(-2001), Eq(std::to_string(-2001)));
}

TEST(XI_TESTSUITE, UnsignedLexicalCast) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_THAT(lexical_cast<uint16_t>("0"), Eq(0u));
  EXPECT_THAT(lexical_cast<uint16_t>("77"), Eq(77u));
  EXPECT_THAT(lexical_cast<uint16_t>("65535"), Eq(65535u));
  EXPECT_ANY_THROW(lexical_cast<uint16_t>("65546"));
  EXPECT_ANY_THROW(lexical_cast<uint16_t>("AC"));
  EXPECT_ANY_THROW(lexical_cast<uint16_t>(""));

  EXPECT_THAT(lexical_cast<uint32_t>("0"), Eq(0u));
  EXPECT_THAT(lexical_cast<uint32_t>("77"), Eq(77u));
  EXPECT_THAT(lexical_cast<uint32_t>("4294967295"), Eq(4294967295u));
  EXPECT_ANY_THROW(lexical_cast<uint32_t>("4294967296"));
  EXPECT_ANY_THROW(lexical_cast<uint32_t>("AC"));
  EXPECT_ANY_THROW(lexical_cast<uint32_t>(""));

  EXPECT_THAT(lexical_cast<uint64_t>("0"), Eq(0u));
  EXPECT_THAT(lexical_cast<uint64_t>("77"), Eq(77u));
  EXPECT_THAT(lexical_cast<uint64_t>("4294967296"), Eq(4294967296ull));
  EXPECT_ANY_THROW(lexical_cast<uint64_t>("AC"));
  EXPECT_ANY_THROW(lexical_cast<uint64_t>(""));
}

TEST(XI_TESTSUITE, SafeLexicalCast) {
  using namespace ::Xi;
  EXPECT_TRUE(safe_lexical_cast<uint16_t>("200"));
  EXPECT_FALSE(safe_lexical_cast<uint16_t>("65546"));
  EXPECT_FALSE(safe_lexical_cast<uint16_t>("AC"));
  EXPECT_FALSE(safe_lexical_cast<uint16_t>(""));
}
