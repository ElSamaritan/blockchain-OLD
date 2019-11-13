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

#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdlib.h>

void *xi_memory_aligned_alloc(size_t size, size_t alignment);
void xi_memory_aligned_free(void *ptr);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

#include <memory>
#include <cinttypes>

namespace Xi {
namespace Memory {

template <class T, size_t N> class AlignedAllocator {
 public:
  typedef T value_type;
  typedef T &reference;
  typedef const T &const_reference;
  typedef T *pointer;
  typedef const T *const_pointer;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  template <class U> struct rebind { typedef AlignedAllocator<U, N> other; };

  inline AlignedAllocator() noexcept {
  }
  inline AlignedAllocator(const AlignedAllocator &) noexcept {
  }

  template <class U> inline AlignedAllocator(const AlignedAllocator<U, N> &) noexcept {
  }

  inline ~AlignedAllocator() noexcept {
  }

  inline pointer address(reference r) {
    return &r;
  }
  inline const_pointer address(const_reference r) const {
    return &r;
  }

  pointer allocate(size_type n, typename std::allocator<void>::const_pointer = 0) {
    pointer res = reinterpret_cast<pointer>(xi_memory_aligned_alloc(sizeof(T) * n, N));
    if (res == nullptr) {
      throw std::bad_alloc{};
    }
    return res;
  }

  inline void deallocate(pointer p, size_type) {
    xi_memory_aligned_free(p);
  }

  inline void construct(pointer p, const_reference value) {
    new (p) value_type(value);
  }
  inline void destroy(pointer p) {
    p->~value_type();
  }

  inline size_type max_size() const noexcept {
    return size_type(-1) / sizeof(T);
  }

  inline bool operator==(const AlignedAllocator &) {
    return true;
  }
  inline bool operator!=(const AlignedAllocator &rhs) {
    return !operator==(rhs);
  }
};

}  // namespace Memory
}  // namespace Xi

#endif
