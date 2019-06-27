/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

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
