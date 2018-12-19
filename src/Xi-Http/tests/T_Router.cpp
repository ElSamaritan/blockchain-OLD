#include <gmock/gmock.h>
#include <Xi/Http/Router.h>

#include <Xi/Http/FunctorRequestHandler.h>
#include <Xi/Http/BasicEndpoint.h>

#define XI_TESTSUITE T_Xi_Http_Router

TEST(XI_TESTSUITE, NotFound) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  Router router{};

  router.onGet("/", [](const auto&) { return Response{}; });

  EXPECT_TRUE(router.acceptsRequest(Request{"/"}));
  EXPECT_FALSE(router.acceptsRequest(Request{"/", Method::Post}));
  EXPECT_EQ(router(Request{"/", Method::Patch}).status(), StatusCode::NotFound);
}

TEST(XI_TESTSUITE, EchoPost) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  Router router{};

  router.onPost("/", [](const auto& request) { return Response{StatusCode::Ok, request.body()}; });

  const std::string testBody{"Xi-Test"};
  Request request{"/", Method::Post};
  request.setBody(testBody);
  auto response = router(request);
  EXPECT_EQ(response.status(), StatusCode::Ok);
  EXPECT_STRCASEEQ(response.body().c_str(), request.body().c_str());
}

TEST(XI_TESTSUITE, HandlesPriority) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  Router router{};

  auto lowPrioEndpoint = std::make_shared<BasicEndpoint>(
      "/", Method::Delete,
      std::make_shared<FunctorRequestHandler>([](const auto&) { return Response{StatusCode::Forbidden}; }));
  auto highPrioEndpoint = std::make_shared<BasicEndpoint>(
      "/", Method::Delete,
      std::make_shared<FunctorRequestHandler>([](const auto&) { return Response{StatusCode::Found}; }));

  router.addEndpoint(highPrioEndpoint, 500);
  router.addEndpoint(lowPrioEndpoint, 499);

  EXPECT_EQ(router(Request{"/", Method::Delete}).status(), StatusCode::Found);
}
