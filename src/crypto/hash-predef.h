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

#pragma once

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

// STATIC
#if !defined(STATIC)
#if defined(_MSC_VER)
#define STATIC static
#else  // _MSC_VER
#define STATIC static
#endif  // !_MSC_VER
#endif  // !defined(STATIC)

// INLINE
#if !defined(INLINE)
#if defined(_MSC_VER)
#define INLINE __inline
#else  // _MSC_VER
#define INLINE inline
#endif  // !_MSC_VER
#endif

// ASM
#if !defined(ASM)
#if defined(__INTEL_COMPILER)
#define ASM __asm__
#elif !defined(_MSC_VER)
#define ASM __asm__
#else  // !_MSC_VER
#define ASM __asm
#endif  // _MSC_VER
#endif  // !defined(ASM)

// THREADV
#if !defined(THREADV)
#if defined(_MSC_VER)
#define THREADV __declspec(thread)
#else  // defined(_MSC_VER)
#define THREADV __thread
#endif  // !defined(_MSC_VER)
#endif  // !defined(THREADV)

// RDATA_ALIGN16
#if !defined(RDATA_ALIGN16)
#if defined(_MSC_VER)
#define RDATA_ALIGN16 __declspec(align(16))
#else  // defined(_MSC_VER)
#define RDATA_ALIGN16 __attribute__((aligned(16)))
#endif  // !defined(_MSC_VER)
#endif  // !defined(RDATA_ALIGN16)

#if defined(__cplusplus)
}
#endif  // __cplusplus
