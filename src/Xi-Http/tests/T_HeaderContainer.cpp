#include <gmock/gmock.h>
#include <Xi/Http/HeaderContainer.h>

#include <string>

#define XI_TESTSUITE T_Xi_Http_HeaderContainer

TEST(XI_TESTSUITE, RequiredAuthenticationSchema) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  HeaderContainer headers{};
  EXPECT_FALSE(headers.requiredAuthenticationScheme());
  headers.setRequiredAuthenticationScheme(AuthenticationType::Basic);
  ASSERT_TRUE(headers.requiredAuthenticationScheme());
  EXPECT_EQ(*headers.requiredAuthenticationScheme(), AuthenticationType::Basic);
}

TEST(XI_TESTSUITE, BasicAuthorization) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  HeaderContainer headers{};
  EXPECT_FALSE(headers.basicAuthorization());
  headers.setBasicAuthorization("xi-user", "my-secret");
  ASSERT_TRUE(headers.basicAuthorization());
  EXPECT_EQ(headers.basicAuthorization()->username(), "xi-user");
  EXPECT_EQ(headers.basicAuthorization()->password(), "my-secret");
  EXPECT_ANY_THROW(headers.setBasicAuthorization("xi:user"));
}

TEST(XI_TESTSUITE, AcceptedContentEncoding) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  HeaderContainer headers{};
  headers.setAcceptedContentEncodings({ContentEncoding::Identity});
  ASSERT_TRUE(headers.get(HeaderContainer::AcceptEncoding));
  EXPECT_STRCASEEQ(headers.get(HeaderContainer::AcceptEncoding)->c_str(), "identity");
  auto accepted = headers.acceptedContentEncodings();
  ASSERT_TRUE(accepted);
  ASSERT_THAT(accepted->size(), Eq(1));
  EXPECT_THAT(*accepted, Contains(ContentEncoding::Identity));

  headers.setAcceptedContentEncodings({ContentEncoding::Gzip, ContentEncoding::Identity});
  ASSERT_TRUE(headers.get(HeaderContainer::AcceptEncoding));
  EXPECT_STRCASEEQ(headers.get(HeaderContainer::AcceptEncoding)->c_str(), "gzip, identity");
  accepted = headers.acceptedContentEncodings();
  ASSERT_TRUE(accepted);
  ASSERT_THAT(accepted->size(), Eq(2));
  EXPECT_THAT(*accepted, Contains(ContentEncoding::Identity));
  EXPECT_THAT(*accepted, Contains(ContentEncoding::Gzip));

  headers.setAcceptedContentEncodings({ContentEncoding::Identity, ContentEncoding::Gzip});
  ASSERT_TRUE(headers.get(HeaderContainer::AcceptEncoding));
  EXPECT_STRCASEEQ(headers.get(HeaderContainer::AcceptEncoding)->c_str(), "identity, gzip");
  accepted = headers.acceptedContentEncodings();
  ASSERT_TRUE(accepted);
  ASSERT_THAT(accepted->size(), Eq(2));
  EXPECT_THAT(*accepted, Contains(ContentEncoding::Identity));
  EXPECT_THAT(*accepted, Contains(ContentEncoding::Gzip));
}
