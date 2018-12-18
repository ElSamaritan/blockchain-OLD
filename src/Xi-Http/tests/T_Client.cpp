#include <gmock/gmock.h>
#include <Xi/Http/Client.h>

#include <vector>

#define XI_TESTSUITE T_Xi_Http_Client

TEST(XI_TESTSUITE, GetRequest) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  Client client{"google.de", 443};
  const uint16_t NumRequests = 1;
  std::vector<std::future<Response>> responses;
  for (auto i = 0u; i < NumRequests; ++i) {
    responses.emplace_back(client.send(Request{}));
  }
  for (auto& fresp : responses) {
    auto const response = fresp.get();
    EXPECT_EQ(response.status(), StatusCode::Ok);
    EXPECT_THAT(response.body().size(), Gt(0));
  }
}
