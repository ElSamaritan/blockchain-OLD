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

#include <vector>
#include <memory>

#include <Xi/TypeSafe/Flag.hpp>
#include <Serialization/BinaryInputStreamSerializer.h>
#include <Serialization/BinaryOutputStreamSerializer.h>
#include <Serialization/JsonInputStreamSerializer.h>
#include <Serialization/JsonOutputStreamSerializer.h>
#include <Serialization/FlagSerialization.hpp>
#include <Serialization/SerializationTools.h>
#include <CryptoNoteCore/CryptoNoteTools.h>

static_assert(CryptoNote::Impl::getFlagShift<1 << 0>() == 0, "");
static_assert(CryptoNote::Impl::getFlagShift<1 << 1>() == 1, "");
static_assert(CryptoNote::Impl::getFlagShift<1 << 2>() == 2, "");
static_assert(CryptoNote::Impl::getFlagShift<1 << 3>() == 3, "");
static_assert(CryptoNote::Impl::getFlagShift<1 << 4>() == 4, "");
static_assert(CryptoNote::Impl::getFlagShift<1 << 5>() == 5, "");
static_assert(CryptoNote::Impl::getFlagShift<1 << 6>() == 6, "");
static_assert(CryptoNote::Impl::getFlagShift<1 << 7>() == 7, "");
static_assert(CryptoNote::Impl::getFlagShift<1 << 8>() == 8, "");
static_assert(CryptoNote::Impl::getFlagShift<1 << 9>() == 9, "");
static_assert(CryptoNote::Impl::getFlagShift<1 << 10>() == 10, "");
static_assert(CryptoNote::Impl::getFlagShift<1 << 11>() == 11, "");
static_assert(CryptoNote::Impl::getFlagShift<1 << 12>() == 12, "");
static_assert(CryptoNote::Impl::getFlagShift<1 << 13>() == 13, "");

namespace CNSerialiaztion_TestSuite {

enum struct StandardFlag {
  First = 1 << 0,
  Second = 1 << 2,
  Third = 1 << 3,
  SecondByte = 1 << 7,

  None = 0,
  FirstSecond = First | Second,
  NotFound = 1 << 4,
  OutOfRange = 1 << 8
};

XI_TYPESAFE_FLAG_MAKE_OPERATIONS(StandardFlag)
XI_SERIALIZATION_FLAG(StandardFlag)

struct TestStruct {
  StandardFlag first;
  StandardFlag second;
  StandardFlag third;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(first)
  KV_MEMBER(second)
  KV_MEMBER(third)
  KV_END_SERIALIZATION
};

}  // namespace CNSerialiaztion_TestSuite

XI_SERIALIZATION_FLAG_RANGE(CNSerialiaztion_TestSuite::StandardFlag, First, SecondByte)
XI_SERIALIZATION_FLAG_TAG(CNSerialiaztion_TestSuite::StandardFlag, First, "first")
XI_SERIALIZATION_FLAG_TAG(CNSerialiaztion_TestSuite::StandardFlag, Second, "second")
XI_SERIALIZATION_FLAG_TAG(CNSerialiaztion_TestSuite::StandardFlag, Third, "third")
XI_SERIALIZATION_FLAG_TAG(CNSerialiaztion_TestSuite::StandardFlag, SecondByte, "second_byte")

TEST(CryptoNote_Serialization, BinaryFlags) {
  using namespace CNSerialiaztion_TestSuite;
  using namespace CryptoNote;
  using namespace testing;

  TestStruct legit{StandardFlag::First, StandardFlag::Second, StandardFlag::FirstSecond};
  BinaryArray seriealized;
  ASSERT_TRUE(toBinaryArray(legit, seriealized));
  EXPECT_THAT(seriealized, SizeIs(3));
  TestStruct deserialized;
  ASSERT_TRUE(fromBinaryArray(deserialized, seriealized));

  EXPECT_EQ(legit.first, deserialized.first);
  EXPECT_EQ(legit.second, deserialized.second);
  EXPECT_EQ(legit.third, deserialized.third);

  EXPECT_TRUE(hasFlag(deserialized.third, StandardFlag::First));
  EXPECT_TRUE(hasFlag(deserialized.third, StandardFlag::Second));

  {
    TestStruct notLegit = legit;
    notLegit.first = StandardFlag::NotFound;
    EXPECT_FALSE(toBinaryArray(notLegit, seriealized));
  }

  {
    TestStruct notLegit = legit;
    notLegit.first |= StandardFlag::NotFound;
    EXPECT_FALSE(toBinaryArray(notLegit, seriealized));
  }

  {
    TestStruct notLegit = legit;
    notLegit.first |= StandardFlag::OutOfRange;
    EXPECT_FALSE(toBinaryArray(notLegit, seriealized));
  }

  {
    TestStruct notLegit = legit;
    notLegit.first = StandardFlag::OutOfRange;
    EXPECT_FALSE(toBinaryArray(notLegit, seriealized));
  }

  legit.first = StandardFlag::SecondByte;
  ASSERT_TRUE(toBinaryArray(legit, seriealized));
  EXPECT_THAT(seriealized, SizeIs(4));

  legit.second = StandardFlag::SecondByte;
  ASSERT_TRUE(toBinaryArray(legit, seriealized));
  EXPECT_THAT(seriealized, SizeIs(5));

  legit.third = StandardFlag::SecondByte;
  ASSERT_TRUE(toBinaryArray(legit, seriealized));
  EXPECT_THAT(seriealized, SizeIs(6));
}

TEST(CryptoNote_Serialization, JsonFlags) {
  using namespace CNSerialiaztion_TestSuite;
  using namespace CryptoNote;
  using namespace testing;

  TestStruct legit{StandardFlag::First, StandardFlag::Second, StandardFlag::FirstSecond};
  std::string seriealized;
  ASSERT_TRUE(storeToJson(legit, seriealized));
  TestStruct deserialized;
  ASSERT_TRUE(loadFromJson(deserialized, seriealized));

  EXPECT_EQ(legit.first, deserialized.first);
  EXPECT_EQ(legit.second, deserialized.second);
  EXPECT_EQ(legit.third, deserialized.third);

  EXPECT_TRUE(hasFlag(deserialized.third, StandardFlag::First));
  EXPECT_TRUE(hasFlag(deserialized.third, StandardFlag::Second));

  {
    TestStruct notLegit = legit;
    notLegit.first = StandardFlag::NotFound;
    EXPECT_FALSE(storeToJson(notLegit, seriealized));
  }

  {
    TestStruct notLegit = legit;
    notLegit.first |= StandardFlag::NotFound;
    EXPECT_FALSE(storeToJson(notLegit, seriealized));
  }

  {
    TestStruct notLegit = legit;
    notLegit.first |= StandardFlag::OutOfRange;
    EXPECT_FALSE(storeToJson(notLegit, seriealized));
  }

  {
    TestStruct notLegit = legit;
    notLegit.first = StandardFlag::OutOfRange;
    EXPECT_FALSE(storeToJson(notLegit, seriealized));
  }
}
