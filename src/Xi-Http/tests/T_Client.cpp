#include <gmock/gmock.h>
#include <Xi/Http/Client.h>

#include <vector>

#define XI_TESTSUITE DISABLED_T_Xi_Http_Client

TEST(XI_TESTSUITE, HTTPGetRequestWithRedirection) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  SSLClientConfiguration config;
  config.Disabled = true;

  Client client{"google.de", 80, config};
  const uint16_t NumRequests = 1;
  std::vector<std::future<Response>> responses;
  for (auto i = 0u; i < NumRequests; ++i) {
    responses.emplace_back(client.send(Request{""}));
  }
  for (auto& fresp : responses) {
    auto const response = fresp.get();
    EXPECT_EQ(response.status(), StatusCode::Ok);
    EXPECT_THAT(response.body().size(), Gt(0));
  }
}

TEST(XI_TESTSUITE, HTTPSGetRequestWithRedirection) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  SSLClientConfiguration config;
  config.VerifyPeers = false;

  Client client{"github.com", 443, config};
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
