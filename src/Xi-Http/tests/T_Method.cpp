#include <gmock/gmock.h>
#include <Xi/Http/Method.h>

#include <string>

#define XI_TESTSUITE T_Xi_Http_Method

TEST(XI_TESTSUITE, ToString) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_NO_THROW(to_string(static_cast<Http::Method>(-1)));
  EXPECT_THAT(to_string(static_cast<Http::Method>(-1)), Eq("UNKNOWN"));
  EXPECT_THAT(to_string(Xi::Http::Method::Get), Eq("GET"));
  EXPECT_THAT(to_string(Xi::Http::Method::Post), Eq("POST"));
  EXPECT_THAT(to_string(Xi::Http::Method::Put), Eq("PUT"));
}
