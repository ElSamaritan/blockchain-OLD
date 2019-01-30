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

#include "crypto/cnx/distribution.h"

#include <map>

namespace {
const std::map<uint32_t, uint32_t> __DistributionMap = {
    {10495u, 0u},       {82047u, 1u},       {341631u, 2u},      {1011967u, 3u},     {2531711u, 4u},
    {5443071u, 5u},     {10498431u, 6u},    {18535679u, 7u},    {30565887u, 8u},    {47754879u, 9u},
    {71188607u, 10u},   {101685631u, 11u},  {140316543u, 12u},  {187883391u, 13u},  {244974591u, 14u},
    {311987839u, 15u},  {389023871u, 16u},  {476072319u, 17u},  {572955263u, 18u},  {678760447u, 19u},
    {793123711u, 20u},  {914963071u, 21u},  {1043530623u, 22u}, {1177561343u, 23u}, {1316089343u, 24u},
    {1458149887u, 25u}, {1602241663u, 26u}, {1747630719u, 27u}, {1892541439u, 28u}, {2036910847u, 29u},
    {2179257087u, 30u}, {2318831359u, 31u}, {2454748671u, 32u}, {2586797311u, 33u}, {2713942143u, 34u},
    {2836339711u, 35u}, {2953065215u, 36u}, {3064208767u, 37u}, {3169393151u, 38u}, {3268712191u, 39u},
    {3362249087u, 40u}, {3449738751u, 41u}, {3531457791u, 42u}, {3607387903u, 43u}, {3677907071u, 44u},
    {3742981759u, 45u}, {3802907007u, 46u}, {3858078335u, 47u}, {3908675327u, 48u}, {3954878463u, 49u},
    {3997050623u, 50u}, {4035563775u, 51u}, {4070377855u, 52u}, {4101962239u, 53u}, {4130452223u, 54u},
    {4156084095u, 55u}, {4179260287u, 56u}, {4200005375u, 57u}, {4218575999u, 58u}, {4235236223u, 59u},
    {4250069887u, 60u}, {4263404799u, 61u}, {4275229951u, 62u}, {4285656831u, 63u}, {4294967295u, 64u},
};
}

uint32_t Crypto::CNX::get_soft_shell_index(uint32_t random) {
  const auto lowerBound = __DistributionMap.lower_bound(random);
  if (lowerBound == __DistributionMap.end())
    return 0;
  else
    return lowerBound->second;
}
