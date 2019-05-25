/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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
