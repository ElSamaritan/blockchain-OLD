#include <gmock/gmock.h>
#include <Xi/Http/Response.h>

#define XI_TESTSUITE T_Xi_Http_Response

TEST(XI_TESTSUITE, Redirection) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  Response res{};
  res.setStatus(StatusCode::TemporaryRedirect);
  EXPECT_TRUE(res.isRedirection());
  res.setStatus(StatusCode::PermanentRedirect);
  EXPECT_TRUE(res.isRedirection());
  res.setStatus(StatusCode::Found);
  EXPECT_TRUE(res.isRedirection());
  res.setStatus(StatusCode::SeeOther);
  EXPECT_TRUE(res.isRedirection());

  res.setStatus(StatusCode::NotFound);
  EXPECT_FALSE(res.isRedirection());
  res.setStatus(StatusCode::Ok);
  EXPECT_FALSE(res.isRedirection());
}
