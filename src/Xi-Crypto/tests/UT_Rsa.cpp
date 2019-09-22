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

#include <Xi/Crypto/Rsa.hh>

#include <random>
#include <memory>
#include <algorithm>
#include <string>

#include <Xi/ExternalIncludePush.h>
#include <gmock/gmock.h>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Encoding/Base64.h>
#include <Xi/Crypto/Random/Random.hh>

namespace {
const std::string RsaSecretKey = R"(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAxOWlvPETgG7QH/QxPOiIbxssL/KoGtq7OVvmrt//RqMV2ue8
bTEzI8xsmfw3uOMydN4n6IdK5L7i/xGbEwp3OWqcj2cGcp9SWL/GcPu2kGqf784U
t2ZTSbnsK8ZaL5icAbloxdHLYcC1CCyfR0cntpkyv2gQRUZ+Iba5iYIgSQWiJiWD
ndc7jfZEIrb0wlsiMuG7GBI7Aiwh0M59SyM2ANsxc8LQu0NRL4TRphdTqvEWs6LE
313llDixroW8IrbAH+cVnXfSxfogRItzWPNRsd9/JJx8RxDCEX6jOOxTz5DJdU8B
6SH6kGffyBz9GhrDmjsIGMMaQvUTUkam9ju1HQIDAQABAoIBAQCL8X0fEWl++0PJ
slHcrJlr/rAlVB7PF2/CkOMgn+xcVEWQVOzD3xgt9Xmq6GziZ/jQv6U1zBMbCeFk
0SqbGfErSe8ErYETrtFkasmsO/vCIUaTtrFauMuxtwd5BX3MHnejchhZoYcgb0s8
CBsdGiHL0o8TI++MjVHQvIKajXZM7weX+NtxOvPpF9Olg2nrRWMkIqLPxcoA588+
5DHflGgZ+8Amx5jk37Q3S2vvhYB1oL1UUJ06F+7tUxtReJGxkG9MOzoyI8bRK3G1
x8EEn0k5o+6/X4YUzobrlt2R2w/5Z7TVE5ZKqW9aOH00iDQN2AJt6S3RhL8tlB+Q
Klwz5YaBAoGBAPYODNoYLtZWGQMWiLUHZ19QtGOaQ6qlVpg3N/OmFZfRZMwojMu5
r4q9OufVdzzBAMoOPxTazBRyIyAaTZ+dAukOxNq9xhtNiVhl8K8yRn2URlRunSNF
5pDHpwWVrvX8ivg5NraL7MH43e9TYQvixI+9qrN+NwqjOA4iicyHP5AVAoGBAMza
9Q1FyDMSvDsERY4Few69pwb5VHTvJcbxCs4gChSN7VwRl9lHLJ8qxx1Zusy+41Wy
kaWgbXNHIzeNeIc4J4N1ldQuNYaNM+RgKxiAGmKWN2c8XwjfSNx8Wdc7HxtddKsy
3ATDGpDWGvJii8A0CfCPGG7WX2LoSdHJeyRaLcrpAoGBAIMuInP0CKSE6njW/rNk
6xvJ7yJbEFBeLbihV9cyaLi0DkU6KTZEHO6ezFTlXdiqHT2cMs1viPYtk8R9wXyj
PPX7xb/JBWFxCRRz5MnCyAoTjxtcjbJMhgwSJV2427qRlZWZCy5vkx4nDEuX7Fll
4N2xhNyFIdsuAainiUfdo2mxAoGBAIRiO1qIbggTY3EAIbLV9+Bez0HCfsHSiic2
PjVWny0JSwrkk6zmQf/aNTLSedU2xtLqwxS1yAVnQ2rzEaDQuThLFLg55H0/YvmC
7Fa48lQjn0onyYKCSSOzeJuOqVuV2X+JLoYlT1WqB1/8LzPWbVX4I2H9R0cztgDM
o7eqT02BAoGAXFL6Pn+JgMRsFyWTqRiy64HDFoirFsEcJJ08KtQxBNlZQdtgjRYE
DmUAZIByXZNMQWk6e6KA4GgwgShrLeUIa84UcBfsZdQkJquo1O54VgckoVfr90fH
f3BcxEA1KGO0n8TvaLix+HLu9qpDlTiLATjQ3Gd2GtzVawXdyVUo15A=
-----END RSA PRIVATE KEY-----
)";

const std::string RsaPublicKey = R"(
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAxOWlvPETgG7QH/QxPOiI
bxssL/KoGtq7OVvmrt//RqMV2ue8bTEzI8xsmfw3uOMydN4n6IdK5L7i/xGbEwp3
OWqcj2cGcp9SWL/GcPu2kGqf784Ut2ZTSbnsK8ZaL5icAbloxdHLYcC1CCyfR0cn
tpkyv2gQRUZ+Iba5iYIgSQWiJiWDndc7jfZEIrb0wlsiMuG7GBI7Aiwh0M59SyM2
ANsxc8LQu0NRL4TRphdTqvEWs6LE313llDixroW8IrbAH+cVnXfSxfogRItzWPNR
sd9/JJx8RxDCEX6jOOxTz5DJdU8B6SH6kGffyBz9GhrDmjsIGMMaQvUTUkam9ju1
HQIDAQAB
-----END PUBLIC KEY-----
)";

const std::string RsaPublicKeyAlternative = R"(
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAwJ7GHcv0lal26M9oJMon
uhknlyYu1Kzs1RzPUdQvmVOTwDJ7kczktAAAgdb6VpH1dJdTaVHcyitoeGozdlyx
KuXWNCFzAneuxZfH7Y4PVAhb4lgWFKYq2xEZRL4zVIWsEQdMX6NX6GHnFes14cLN
ClQmOZNXZDyI710+UL69IBpJexps1VWiz+1tTvCTvNLiuJ057GX14c4IhS4ItyM+
u1yEGpmd84+7O5Fi0O/tMeVSXcQjKOT37u/QZMsjgaHZnx5E0DvD/K7blWy0Iu79
8qvtMC1OUuMSxVH8fxJjNcY5B0QrKhWcMIwOXTESShd19BEntzvlsSJDtxz8CWms
YQIDAQAB
-----END PUBLIC KEY-----
)";

const std::string Peers = R"([
    "xi://galaxia.hopto.org"
])";

const std::string PeersSignature =
    "bmdvbOSmZsvlAObFc5GruCGczX3vWK0hXqaEoqtOB3QgkKiOX+DJtJ05adjiTxldSV4b8yMYJn+viIj6xq0HAs7GUmALtJT8JaO/"
    "6IHoycFXK0qe86RykC5jJc3cKdzNaqz31ewHgYE0w3JjZ+6PmJXcnBuL/"
    "lEMppG3BPsGmFGfN1DzIMenShO8AmF3CU9iRnDUpVZo39oUIH29Jib1+"
    "aeoMuq8UTJhLQrKT8lQUs4B0BD9MLvZf6oxzgG0Kv6leyhv8EVD6MS0xWNw1w/Puk0p5k+8kVv9To4t/3ZC1iE1UPYlLSlrl5/"
    "ogEV0kurOC93EAuxbrJb4K6L9LkMWiQ==";
}  // namespace

#define XI_TESTSUITE T_Xi_Crypto_Rsa

class XI_TESTSUITE : public ::testing::Test {
 public:
  Xi::ConstByteSpan secKey{Xi::asConstByteSpan(RsaSecretKey)};
  Xi::ConstByteSpan pubKey{Xi::asConstByteSpan(RsaPublicKey)};
  Xi::ConstByteSpan pubKeyAlternative{Xi::asConstByteSpan(RsaPublicKeyAlternative)};

  std::vector<Xi::ByteVector> messages{};

  void SetUp() override {
    std::default_random_engine eng{};
    messages.emplace_back(Xi::ByteVector{});
    for (size_t i = 1; i < 16; ++i) {
      std::uniform_int_distribution<size_t> dist{1ULL << i, 1ULL << (i + 1ULL)};
      for (size_t j = 0; j < 4; ++j) {
        messages.emplace_back(Xi::Crypto::Random::generate(dist(eng)).takeOrThrow());
      }
    }
  }
};

TEST_F(XI_TESTSUITE, ValidCases) {
  using namespace Xi::Crypto::Rsa;

  for (const auto& message : messages) {
    const auto signature = sign(message, secKey);
    ASSERT_FALSE(signature.isError());
    EXPECT_TRUE(verify(message, pubKey, *signature));
  }
}

TEST_F(XI_TESTSUITE, PeersSignature) {
  using namespace Xi::Crypto::Rsa;
  const auto seedSignature = Xi::Base64::decode(PeersSignature);
  EXPECT_TRUE(verify(Xi::asConstByteSpan(Peers), pubKey, Xi::asConstByteSpan(seedSignature)));
}

TEST_F(XI_TESTSUITE, AlteredMessage) {
  using namespace Xi::Crypto::Rsa;
  std::default_random_engine eng{};

  for (const auto& message : messages) {
    if (message.empty()) {
      continue;
    }
    const auto signature = sign(message, secKey);
    ASSERT_FALSE(signature.isError());
    std::uniform_int_distribution<size_t> dist{0, message.size() - 1};
    for (size_t i = 0; i < 16; ++i) {
      auto copy = message;
      copy[dist(eng)]++;
      EXPECT_FALSE(verify(copy, pubKey, *signature));
    }
  }
}

TEST_F(XI_TESTSUITE, InvalidSigner) {
  using namespace Xi::Crypto::Rsa;

  for (const auto& message : messages) {
    const auto signature = sign(message, secKey);
    ASSERT_FALSE(signature.isError());
    EXPECT_FALSE(verify(message, pubKeyAlternative, *signature));
  }
}
