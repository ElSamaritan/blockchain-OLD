#include <gmock/gmock.h>
#include <Xi/Http/ContentType.h>

#include <string>

#define XI_TESTSUITE T_Xi_Http_ContentType

TEST(XI_TESTSUITE, ToString) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_ANY_THROW(to_string(static_cast<Http::ContentType>(-1)));
  EXPECT_THAT(to_string(Http::ContentType::Json), Eq("application/json"));
}

TEST(XI_TESTSUITE, LexicalCast) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_ANY_THROW(lexical_cast<Http::ContentType>(""));
  EXPECT_ANY_THROW(lexical_cast<Http::ContentType>("Application/json"));
  EXPECT_ANY_THROW(lexical_cast<Http::ContentType>(" application/json"));
  EXPECT_ANY_THROW(lexical_cast<Http::ContentType>("application/json "));
  EXPECT_EQ(lexical_cast<Http::ContentType>("application/json"), Http::ContentType::Json);
}
