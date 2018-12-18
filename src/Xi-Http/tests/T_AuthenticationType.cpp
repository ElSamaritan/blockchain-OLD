#include <gmock/gmock.h>
#include <Xi/Http/AuthenticationType.h>

#include <string>

#define XI_TESTSUITE T_Xi_Http_AuthenticationType

TEST(XI_TESTSUITE, ToString) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_ANY_THROW(to_string(static_cast<Http::AuthenticationType>(-1)));
  EXPECT_THAT(to_string(Http::AuthenticationType::Basic), Eq("Basic"));
  EXPECT_THAT(to_string(Http::AuthenticationType::Unsupported), Eq("Unsupported"));
}

TEST(XI_TESTSUITE, LexicalCast) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_EQ(lexical_cast<Http::AuthenticationType>("application/json"), Http::AuthenticationType::Unsupported);
  EXPECT_EQ(lexical_cast<Http::AuthenticationType>(""), Http::AuthenticationType::Unsupported);
  EXPECT_EQ(lexical_cast<Http::AuthenticationType>("basic"), Http::AuthenticationType::Unsupported);
  EXPECT_EQ(lexical_cast<Http::AuthenticationType>("Basic "), Http::AuthenticationType::Unsupported);
  EXPECT_EQ(lexical_cast<Http::AuthenticationType>("Basic"), Http::AuthenticationType::Basic);
}
