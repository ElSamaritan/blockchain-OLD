#include <gmock/gmock.h>
#include <Xi/Http/Server.h>

#include <vector>

#include <boost/predef.h>

#include <Xi/Http/Client.h>

#define XI_TESTSUITE DISABLED_T_Xi_Http_Server

using namespace ::Xi::Http;

class MockRequestHandler : public Xi::Http::RequestHandler {
 public:
  ~MockRequestHandler() = default;

  MOCK_METHOD1(doHandleRequest, Response(const Request&));
};

#if BOOST_COMP_MSVC
#pragma warning(push)
#pragma warning(disable : 4100)
#endif
ACTION(ThrowAction) { throw std::runtime_error{""}; }
ACTION(ReturnBody) {
  const Request& request = arg0;
  return Response{StatusCode::Ok, std::string{"Hi "} + request.body()};
}

MATCHER(HasBody, negation ? "has no body" : "has a body") {
  const auto& request = std::get<0>(arg);
  return !request.body().empty();
}

MATCHER_P(HasContentType, ct,
          std::string{negation ? "has not content type " : "has content type "} + Xi::to_string(ct)) {
  const auto& request = std::get<0>(arg);
  return request.headers().contentType() && *request.headers().contentType() == ct;
}
#if BOOST_COMP_MSVC
#pragma warning(pop)
#endif

class XI_TESTSUITE : public ::testing::Test {
 public:
  const uint16_t Port = 48008;
  Server server{};

  SSLClientConfiguration clientConfig;
  Client client{"127.0.0.1", Port, clientConfig};
  std::shared_ptr<MockRequestHandler> mock = std::make_shared<MockRequestHandler>();

  void SetUp() override {
    server.setHandler(mock);
    server.start("0.0.0.0", Port);
  }

  void TearDown() override { server.stop(); }
};

TEST_F(XI_TESTSUITE, HTTPRequest) {
  using namespace ::testing;

  const uint32_t NumRequests = 100;

  ON_CALL(*mock, doHandleRequest).With(HasBody()).WillByDefault(ReturnBody());
  EXPECT_CALL(*mock, doHandleRequest(_)).Times(NumRequests);

  std::vector<std::future<Response>> responses;
  for (uint32_t i = 0; i < NumRequests; ++i)
    responses.emplace_back(client.get("", ContentType::Plain, std::to_string(i)));

  for (uint32_t i = 0; i < NumRequests; ++i) {
    Response response;
    ASSERT_NO_THROW(response = responses[i].get());
    EXPECT_STREQ(response.body().c_str(), (std::string{"Hi "} + std::to_string(i)).c_str());
    EXPECT_EQ(response.status(), StatusCode::Ok);
  }
}

TEST_F(XI_TESTSUITE, HTTPHandlerThrows) {
  using namespace ::testing;

  ON_CALL(*mock, doHandleRequest).With(HasContentType(ContentType::Text)).WillByDefault(ThrowAction());
  EXPECT_CALL(*mock, doHandleRequest(_)).Times(1);

  {
    auto response = client.postSync("", ContentType::Text, "{}");
    EXPECT_EQ(response.status(), StatusCode::InternalServerError);
  }

  {
    auto response = client.postSync("", ContentType::Text, "");
    EXPECT_EQ(response.status(), StatusCode::InternalServerError);
  }
}
