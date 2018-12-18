#include <gmock/gmock.h>
#include <Xi/Http/BasicCredentials.h>

#include <string>

#define XI_TESTSUITE T_Xi_Http_BasicCredentials

TEST(XI_TESTSUITE, Constructor) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  EXPECT_ANY_THROW(BasicCredentials{""});
  EXPECT_ANY_THROW(BasicCredentials{"Xi:User"});

  BasicCredentials cred{"xi-user", "mySecret:_H%%$!"};
  EXPECT_EQ(cred.username(), "xi-user");
  EXPECT_EQ(cred.password(), "mySecret:_H%%$!");
}

TEST(XI_TESTSUITE, SetUsername) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  BasicCredentials cred{"xi-user", "mySecret:_H%%$!"};
  EXPECT_ANY_THROW(cred.setUsername("iam:invalid"));
  EXPECT_EQ(cred.username(), "xi-user");
  EXPECT_ANY_THROW(cred.setUsername(""));
  EXPECT_EQ(cred.username(), "xi-user");
}

TEST(XI_TESTSUITE, ToString) {
  using namespace ::testing;
  using namespace ::Xi;
  using namespace ::Xi::Http;

  BasicCredentials cred{"xi-user", "mySecret:_H%%$!"};
  EXPECT_EQ(to_string(cred), "xi-user:mySecret:_H%%$!");
  EXPECT_EQ(to_string(BasicCredentials{"xi-user"}), "xi-user:");
}

TEST(XI_TESTSUITE, LexicalCast) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  BasicCredentials cred{"xi-user", "mySecret:_H%%$!"};

  {
    BasicCredentials parsedCred = Xi::lexical_cast<BasicCredentials>(Xi::to_string(cred));
    EXPECT_EQ(parsedCred.username(), cred.username());
    EXPECT_EQ(parsedCred.password(), cred.password());
  }

  {
    BasicCredentials parsedCred = Xi::lexical_cast<BasicCredentials>("  myUser:");
    EXPECT_EQ(parsedCred.username(), "  myUser");
    EXPECT_EQ(parsedCred.password(), "");
  }
}
