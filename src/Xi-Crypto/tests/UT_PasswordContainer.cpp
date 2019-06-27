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

#include <Xi/Crypto/PasswordContainer.h>

#include <random>
#include <memory>
#include <algorithm>

#include <Xi/ExternalIncludePush.h>
#include <gmock/gmock.h>
#include <Xi/ExternalIncludePop.h>

#define XI_TESTSUITE T_Xi_Crypto_PasswordContainer

class XI_TESTSUITE : public ::testing::Test {
 public:
  std::string password;
  std::unique_ptr<Xi::Crypto::PasswordContainer> container;

  void SetUp() override {
    std::default_random_engine eng{};
    std::uniform_int_distribution<uint32_t> dist;
    password.resize(32, '\0');
    for (size_t i = 0; i < 32 / sizeof(uint32_t); ++i) {
      *(reinterpret_cast<uint32_t*>(&password[0]) + i) = dist(eng);
    }
    container = std::make_unique<Xi::Crypto::PasswordContainer>(password);
  }
};

TEST_F(XI_TESTSUITE, AcceptsPassword) { EXPECT_TRUE(container->validate(password)); }

TEST_F(XI_TESTSUITE, DeclinesPassword) {
  EXPECT_FALSE(container->validate(std::string{password.rbegin(), password.rend()}));
}
