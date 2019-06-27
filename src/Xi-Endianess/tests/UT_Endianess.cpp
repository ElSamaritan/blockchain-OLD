// Copyright (c) 2019 by Michael Herwig <michael.herwig@hotmail.de>
// All rights reserved.

#include <limits>
#include <random>

#include <gmock/gmock.h>

#include <Xi/Endianess/Endianess.hh>

#define XI_UNIT_TEST_SUITE Endianess

namespace {

template <typename _IntegralT>
static void generalizedTest(_IntegralT value) {
  using namespace Xi::Endianess;

  const auto origin = value;
  const auto swapped = Xi::Endianess::swap(value);
  const auto doubleSwapped = Xi::Endianess::swap(swapped);
  const auto tripleSwapped = Xi::Endianess::swap(doubleSwapped);

  EXPECT_EQ(origin, doubleSwapped);
  EXPECT_EQ(swapped, tripleSwapped);
}

template <typename _IntegralT>
static void generalizedTest() {
  using limits = std::numeric_limits<_IntegralT>;

  const auto min = limits::min();
  const auto zero = 0;
  const auto max = limits::max();

  generalizedTest(min);
  generalizedTest(zero);
  generalizedTest(max);

  std::default_random_engine eng{/* */};
  std::uniform_int_distribution dist{min, max};

  for (size_t i = 0; i < 1024; ++i) {
    generalizedTest(dist(eng));
  }
}

}  // namespace

TEST(XI_UNIT_TEST_SUITE, Int16) {
  generalizedTest<int16_t>();
}

TEST(XI_UNIT_TEST_SUITE, UInt16) {
  generalizedTest<uint16_t>();
}

TEST(XI_UNIT_TEST_SUITE, Int32) {
  generalizedTest<int32_t>();
}

TEST(XI_UNIT_TEST_SUITE, UInt32) {
  generalizedTest<uint32_t>();
}

TEST(XI_UNIT_TEST_SUITE, Int64) {
  generalizedTest<int64_t>();
}

TEST(XI_UNIT_TEST_SUITE, UInt64) {
  generalizedTest<uint64_t>();
}
