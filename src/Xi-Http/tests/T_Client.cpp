#include <gmock/gmock.h>
#include <Xi/Http/Client.h>

#include <vector>

#define XI_TESTSUITE T_Xi_Http_Client

TEST(XI_TESTSUITE, HTTPGetRequestWithRedirection) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  Client client{"google.de", 80, false};
  const uint16_t NumRequests = 1;
  std::vector<std::future<Response>> responses;
  for (auto i = 0u; i < NumRequests; ++i) {
    responses.emplace_back(
        client.send(Request{"/search?source=hp&ei=sFsZXODcDMPSwALwoqygCg&q=test&btnK=Google+Search&oq=test&gs_l=psy-ab."
                            "3..0l10.124.1300..1501...1.0..0.89.360.6......0....1..gws-wiz.....0..35i39.tc0Dx5y_75s"}));
  }
  for (auto& fresp : responses) {
    auto const response = fresp.get();
    EXPECT_EQ(response.status(), StatusCode::Ok);
    EXPECT_THAT(response.body().size(), Gt(0));
  }
}

// TEST(XI_TESTSUITE, HTTPSGetRequestWithRedirection) {
//  using namespace ::testing;
//  using namespace ::Xi::Http;

//  Client client{"google.de", 443, true};
//  const uint16_t NumRequests = 1;
//  std::vector<std::future<Response>> responses;
//  for (auto i = 0u; i < NumRequests; ++i) {
//    responses.emplace_back(client.send(Request{}));
//  }
//  for (auto& fresp : responses) {
//    auto const response = fresp.get();
//    EXPECT_EQ(response.status(), StatusCode::Ok);
//    EXPECT_THAT(response.body().size(), Gt(0));
//  }
//}
