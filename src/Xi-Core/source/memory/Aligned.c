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

#include "Xi/Memory/Aligned.hh"

#include <malloc.h>

#if ((defined __QNXNTO__) || (defined _GNU_SOURCE) ||        \
     ((defined _XOPEN_SOURCE) && (_XOPEN_SOURCE >= 600))) && \
    (defined _POSIX_ADVISORY_INFO) && (_POSIX_ADVISORY_INFO > 0)
#define HAS_POSIX_MEMALIGN 1
#else
#define HAS_POSIX_MEMALIGN 0
#endif

#if defined(__GLIBC__) && ((__GLIBC__ >= 2 && __GLIBC_MINOR__ >= 8) || __GLIBC__ > 2) && \
    defined(__LP64__)
#define GLIBC_MALLOC_ALREADY_ALIGNED 1
#else
#define GLIBC_MALLOC_ALREADY_ALIGNED 0
#endif

#if defined(__FreeBSD__) && !defined(__arm__) && !defined(__mips__)
#define FREEBSD_MALLOC_ALREADY_ALIGNED 1
#else
#define FREEBSD_MALLOC_ALREADY_ALIGNED 0
#endif

#if (defined(__APPLE__) || defined(_WIN64) || GLIBC_MALLOC_ALREADY_ALIGNED || \
     FREEBSD_MALLOC_ALREADY_ALIGNED)
#define MALLOC_ALREADY_ALIGNED 1
#else
#define MALLOC_ALREADY_ALIGNED 0
#endif

#if defined(SSE_INSTR_SET) && SSE_INSTR_SET > 0
#define HAS_MM_MALLOC 1
#else
#define HAS_MM_MALLOC 0
#endif

void *xi_memory_aligned_alloc(size_t size, size_t alignment) {
#if defined(MALLOC_ALREADY_ALIGNED) && MALLOC_ALREADY_ALIGNED
  (void)alignment;
  return malloc(size);
#elif defined(HAS_MM_MALLOC) && HAS_MM_MALLOC
  return _mm_malloc(size, alignment);
#elif defined(HAS_POSIX_MEMALIGN) && HAS_POSIX_MEMALIGN
  void *res;
  const int failed = posix_memalign(&res, size, alignment);
  if (failed)
    res = 0;
  return res;
#elif defined(_MSC_VER)
  return _aligned_malloc(size, alignment);
#else
#pragma error "aligned memory alloctation not supported"
#endif
}

void xi_memory_aligned_free(void *ptr) {
#if defined(MALLOC_ALREADY_ALIGNED) && MALLOC_ALREADY_ALIGNED
  free(ptr);
#elif defined(HAS_MM_MALLOC) && HAS_MM_MALLOC
  _mm_free(ptr);
#elif defined(HAS_POSIX_MEMALIGN) && HAS_POSIX_MEMALIGN
  free(ptr);
#elif defined(_MSC_VER)
  _aligned_free(ptr);
#else
#pragma error "aligned memory alloctation not supported"
#endif
}
