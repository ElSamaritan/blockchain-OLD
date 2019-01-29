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
#include <Xi/Http/Client.h>

#include <vector>

#define XI_TESTSUITE DISABLED_T_Xi_Http_Client

TEST(XI_TESTSUITE, HTTPGetRequestWithRedirection) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  SSLClientConfiguration config;
  config.Disabled = true;

  Client client{"google.de", 80, config};
  const uint16_t NumRequests = 1;
  std::vector<std::future<Response>> responses;
  for (auto i = 0u; i < NumRequests; ++i) {
    responses.emplace_back(client.send(Request{""}));
  }
  for (auto& fresp : responses) {
    auto const response = fresp.get();
    EXPECT_EQ(response.status(), StatusCode::Ok);
    EXPECT_THAT(response.body().size(), Gt(0));
  }
}

TEST(XI_TESTSUITE, HTTPSGetRequestWithRedirection) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  SSLClientConfiguration config;
  config.VerifyPeers = false;

  Client client{"github.com", 443, config};
  const uint16_t NumRequests = 1;
  std::vector<std::future<Response>> responses;
  for (auto i = 0u; i < NumRequests; ++i) {
    responses.emplace_back(client.send(Request{}));
  }
  for (auto& fresp : responses) {
    auto const response = fresp.get();
    EXPECT_EQ(response.status(), StatusCode::Ok);
    EXPECT_THAT(response.body().size(), Gt(0));
  }
}
