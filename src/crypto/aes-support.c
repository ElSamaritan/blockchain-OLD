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

#include "crypto/aes-support.h"

#include <stdlib.h>
#include <string.h>

#include "crypto/hash-predef.h"

#if defined(_MSC_VER)
#include <Windows.h>
#define crypto_aes_support_cpuid(info, x) __cpuidex(info, x, 0)
#else
void crypto_aes_support_cpuid(int CPUInfo[4], int InfoType) {
  ASM __volatile__("cpuid"
                   : "=a"(CPUInfo[0]), "=b"(CPUInfo[1]), "=c"(CPUInfo[2]), "=d"(CPUInfo[3])
                   : "a"(InfoType), "c"(0));
}
#endif

int check_aes_hardware_support(void)
{
  int cpuid_results[4];
  static int supported = -1;

  if (supported >= 0) return supported;

  crypto_aes_support_cpuid(cpuid_results, 1);
  return supported = cpuid_results[2] & (1 << 25);
}

int check_aes_hardware_disabled(void)
{
  static int use = -1;

  if (use != -1) return use;

  const char *env = getenv("XI_USE_SOFTWARE_AES");
  if (!env) {
    use = 0;
  } else if (!strcmp(env, "0") || !strcmp(env, "no")) {
    use = 0;
  } else {
    use = 1;
  }
  return use;
}
