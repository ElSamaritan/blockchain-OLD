/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

#include "hash-allocation.h"

#include <stdlib.h>

#include "crypto/hash-predef.h"

#if defined(_MSC_VER) || defined (__MINGW32__)
#include <Windows.h>
#else
#include <sys/mman.h>
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)
static BOOL SetLockPagesPrivilege(HANDLE hProcess, BOOL bEnable) {
  struct {
    DWORD count;
    LUID_AND_ATTRIBUTES privilege[1];
  } info;

  HANDLE token;
  if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &token)) return FALSE;

  info.count = 1;
  info.privilege[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;

  if (!LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &(info.privilege[0].Luid))) return FALSE;

  if (!AdjustTokenPrivileges(token, FALSE, (PTOKEN_PRIVILEGES)&info, 0, NULL, NULL)) return FALSE;

  if (GetLastError() != ERROR_SUCCESS) return FALSE;

  CloseHandle(token);

  return TRUE;
}
#endif

static THREADV uint8_t* __AllocatedMemory = NULL;
static THREADV uint8_t  __AllocatedOnHeap = 0;
static THREADV uint32_t __AllocatedSize   = 0;

uint8_t *xi_hash_allocate_state(uint32_t pageSize)
{
  // The required page size is likely to change thus we need to check if we already have a sufficient amount of memory.
  if(__AllocatedMemory != NULL) {
    if(pageSize < __AllocatedSize)
      return __AllocatedMemory;
    else
      xi_hash_free_state();
  }

#if defined(_MSC_VER) || defined(__MINGW32__)
  SetLockPagesPrivilege(GetCurrentProcess(), TRUE);
  __AllocatedMemory = (uint8_t *)VirtualAlloc(__AllocatedMemory, pageSize,
                                              MEM_LARGE_PAGES | MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__DragonFly__) || defined(__NetBSD__)
  __AllocatedMemory = mmap(0, pageSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, 0, 0);
#else
  __AllocatedMemory = mmap(0, pageSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, 0, 0);
#endif
  if (__AllocatedMemory == MAP_FAILED) __AllocatedMemory = NULL;
#endif
  if (__AllocatedMemory == NULL) {
    __AllocatedOnHeap = 0;
    __AllocatedMemory = (uint8_t *)malloc(pageSize);
  } else {
    __AllocatedOnHeap = 1;
  }
  __AllocatedSize = __AllocatedMemory == NULL ? 0 : pageSize;
  return __AllocatedMemory;
}

void xi_hash_free_state(void)
{
  if (__AllocatedMemory == NULL) return;

  if (__AllocatedOnHeap) {
#if defined(_MSC_VER) || defined(__MINGW32__)
    VirtualFree(__AllocatedMemory, 0, MEM_RELEASE);
#else
    munmap(__AllocatedMemory, __AllocatedSize);
#endif
  } else {
    free(__AllocatedMemory);
  }
  __AllocatedMemory = NULL;
  __AllocatedSize = 0;
}
