#include <gmock/gmock.h>
#include <Xi/Http/StatusCode.h>

#include <string>

#define XI_TESTSUITE T_Xi_Http_StatusCode

TEST(XI_TESTSUITE, ToString) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_NO_THROW(to_string(static_cast<Http::StatusCode>(-1)));
  EXPECT_THAT(to_string(static_cast<Http::StatusCode>(-1)), Eq("Unknown"));
  EXPECT_THAT(to_string(Xi::Http::StatusCode::Gone), Eq("Gone"));
  EXPECT_THAT(to_string(Xi::Http::StatusCode::NetworkConnectTimeout), Eq("NetworkConnectTimeout"));
}
