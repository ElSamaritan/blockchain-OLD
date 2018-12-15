#include <Xi/Utils/ConstExprMath.h>
#include <gmock/gmock.h>

TEST(T_Xi_Utils_ConstExprMath, pow) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_THAT(Xi::pow(100u, 0u), Eq(1u));
  EXPECT_THAT(Xi::pow(100u, 2u), Eq(10000u));
  EXPECT_THAT(Xi::pow(2u, 8u), Eq(256u));
  EXPECT_THAT(Xi::pow(4u, 4u), Eq(256u));
}
