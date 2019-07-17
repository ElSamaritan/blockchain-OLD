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

#include <limits>
#include <random>
#include <array>
#include <algorithm>
#include <type_traits>

#include <gmock/gmock.h>

#include <Xi/Encoding/VarInt.hh>

#define XI_TEST_SUITE Xi_Encoding_VarInt

namespace {
template <typename _IntT>
void generic_decode_encode_test() {
  using namespace Xi::Encoding::VarInt;

  const _IntT max = std::numeric_limits<_IntT>::max();
  const _IntT min = std::numeric_limits<_IntT>::min();
  const _IntT zero = 0;
  std::array<_IntT, 200> randoms{};
  {
    if constexpr (sizeof(_IntT) == 1) {
      std::default_random_engine e{};
      std::uniform_int_distribution<uint16_t> dist{std::numeric_limits<uint16_t>::min(),
                                                   std::numeric_limits<uint16_t>::max()};
      for (auto &i : randoms) {
        i = static_cast<_IntT>(dist(e));
      }
    } else {
      std::default_random_engine e{};
      std::uniform_int_distribution<_IntT> dist{std::numeric_limits<_IntT>::min(), std::numeric_limits<_IntT>::max()};
      for (auto &i : randoms) {
        i = static_cast<_IntT>(dist(e));
      }
    }
  }

  Xi::ByteArray<32> buffer{};

  const auto testValue = [&buffer](_IntT value) {
    _IntT placeholder;
    buffer.fill(0);
    size_t encodingSize = 0;
    if (auto ec = encode(value, buffer); ec.isError()) {
      FAIL();
    } else {
      encodingSize = *ec;
    }

    if (value == std::numeric_limits<_IntT>::min()) {
      EXPECT_EQ(encodingSize, 1);
    } else if (value == std::numeric_limits<_IntT>::max()) {
      EXPECT_EQ(encodingSize, 2);
    }

    ASSERT_GT(encodingSize, 0u);
    EXPECT_LE(encodingSize, maximumEncodingSize<_IntT>());
    EXPECT_EQ(buffer[encodingSize], 0u);
    EXPECT_FALSE(hasSuccessor(buffer[encodingSize - 1]));
    ASSERT_FALSE(decode(buffer, placeholder).isError());
    EXPECT_EQ(value, placeholder);
    if (encodingSize > 1) {
      for (size_t i = 0; i < encodingSize - 2; ++i) {
        EXPECT_TRUE(hasSuccessor(buffer[i]));
      }
    }
  };

  testValue(min);
  testValue(max);
  testValue(min + 1);
  testValue(max - 1);
  testValue(zero);

  for (_IntT i = 0; i < 124; ++i) {
    testValue(zero + i);
    testValue(max - i);
    if constexpr (std::is_signed_v<_IntT>) {
      testValue(zero - i);
      testValue(min + i);
    }
  }

  for (auto i : randoms) {
    testValue(i);
    testValue(~i);
  }
}

template <typename _IntT>
void generic_out_of_memory_test() {
  using namespace Xi::Encoding::VarInt;

  Xi::ByteArray<1> buffer{0xFF};
  Xi::ByteVector emptyBuffer{};
  ASSERT_EQ(emptyBuffer.size(), 0);

  _IntT placeholder = 0;
  EXPECT_TRUE(decode(buffer, placeholder).isError());
  EXPECT_TRUE(decode(emptyBuffer, placeholder).isError());

  placeholder = std::numeric_limits<_IntT>::max();
  EXPECT_TRUE(encode(placeholder, emptyBuffer).isError());
}

template <typename _IntT>
void generic_overflow_test() {
  using namespace Xi::Encoding::VarInt;

  Xi::ByteArray<32> buffer{};
  _IntT bigNum = std::numeric_limits<_IntT>::max();
  size_t encodingSize = 0;
  if (auto ec = encode(bigNum, buffer); ec.isError()) {
    FAIL();
  } else {
    encodingSize = *ec;
  }
  ASSERT_GT(encodingSize, 0u);
  buffer[encodingSize - 1] = 0xFF;
  EXPECT_TRUE(decode(buffer, bigNum).isError());
}
}  // namespace

TEST(XI_TEST_SUITE, DecodeEncodeInt8) {
  generic_decode_encode_test<int8_t>();
}
TEST(XI_TEST_SUITE, DecodeEncodeUInt8) {
  generic_decode_encode_test<uint8_t>();
}
TEST(XI_TEST_SUITE, DecodeEncodeInt16) {
  generic_decode_encode_test<int16_t>();
}
TEST(XI_TEST_SUITE, DecodeEncodeUInt16) {
  generic_decode_encode_test<uint16_t>();
}
TEST(XI_TEST_SUITE, DecodeEncodeInt32) {
  generic_decode_encode_test<int32_t>();
}
TEST(XI_TEST_SUITE, DecodeEncodeUInt32) {
  generic_decode_encode_test<uint32_t>();
}
TEST(XI_TEST_SUITE, DecodeEncodeInt64) {
  generic_decode_encode_test<int64_t>();
}
TEST(XI_TEST_SUITE, DecodeEncodeUInt64) {
  generic_decode_encode_test<uint64_t>();
}

TEST(XI_TEST_SUITE, OutOfMemoryInt8) {
  generic_out_of_memory_test<int8_t>();
}
TEST(XI_TEST_SUITE, OutOfMemoryUInt8) {
  generic_out_of_memory_test<uint8_t>();
}
TEST(XI_TEST_SUITE, OutOfMemoryInt16) {
  generic_out_of_memory_test<int16_t>();
}
TEST(XI_TEST_SUITE, OutOfMemoryUInt16) {
  generic_out_of_memory_test<uint16_t>();
}
TEST(XI_TEST_SUITE, OutOfMemoryInt32) {
  generic_out_of_memory_test<int32_t>();
}
TEST(XI_TEST_SUITE, OutOfMemoryUInt32) {
  generic_out_of_memory_test<uint32_t>();
}
TEST(XI_TEST_SUITE, OutOfMemoryInt64) {
  generic_out_of_memory_test<int64_t>();
}
TEST(XI_TEST_SUITE, OutOfMemoryUInt64) {
  generic_out_of_memory_test<uint64_t>();
}

TEST(XI_TEST_SUITE, OverflowInt8) {
  generic_overflow_test<int8_t>();
}
TEST(XI_TEST_SUITE, OverflowUInt8) {
  generic_overflow_test<uint8_t>();
}
TEST(XI_TEST_SUITE, OverflowInt16) {
  generic_overflow_test<int16_t>();
}
TEST(XI_TEST_SUITE, OverflowUInt16) {
  generic_overflow_test<uint16_t>();
}
TEST(XI_TEST_SUITE, OverflowInt32) {
  generic_overflow_test<int32_t>();
}
TEST(XI_TEST_SUITE, OverflowUInt32) {
  generic_overflow_test<uint32_t>();
}
TEST(XI_TEST_SUITE, OverflowInt64) {
  generic_overflow_test<int64_t>();
}
TEST(XI_TEST_SUITE, OverflowUInt64) {
  generic_overflow_test<uint64_t>();
}
