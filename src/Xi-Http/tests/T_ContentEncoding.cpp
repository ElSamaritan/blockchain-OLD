#include <gmock/gmock.h>
#include <Xi/Http/ContentEncoding.h>

#include <string>

#define XI_TESTSUITE T_Xi_Http_ContentEncoding

TEST(XI_TESTSUITE, ToString) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_ANY_THROW(to_string(static_cast<Http::ContentEncoding>(-1)));
  EXPECT_THAT(to_string(Http::ContentEncoding::Gzip), Eq("gzip"));
  EXPECT_THAT(to_string(Http::ContentEncoding::Identity), Eq("identity"));
  EXPECT_THAT(to_string(Http::ContentEncoding::Deflate), Eq("deflate"));
  EXPECT_THAT(to_string(Http::ContentEncoding::Brotli), Eq("br"));
  EXPECT_THAT(to_string(Http::ContentEncoding::Compress), Eq("compress"));
}

TEST(XI_TESTSUITE, LexicalCast) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_ANY_THROW(lexical_cast<Http::ContentEncoding>(""));
  EXPECT_ANY_THROW(lexical_cast<Http::ContentEncoding>("GZIP"));
  EXPECT_ANY_THROW(lexical_cast<Http::ContentEncoding>(" gzip"));
  EXPECT_ANY_THROW(lexical_cast<Http::ContentEncoding>("br "));
  EXPECT_EQ(lexical_cast<Http::ContentEncoding>("gzip"), Http::ContentEncoding::Gzip);
  EXPECT_EQ(lexical_cast<Http::ContentEncoding>("compress"), Http::ContentEncoding::Compress);
  EXPECT_EQ(lexical_cast<Http::ContentEncoding>("deflate"), Http::ContentEncoding::Deflate);
  EXPECT_EQ(lexical_cast<Http::ContentEncoding>("identity"), Http::ContentEncoding::Identity);
  EXPECT_EQ(lexical_cast<Http::ContentEncoding>("br"), Http::ContentEncoding::Brotli);
}
