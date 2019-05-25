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

#include <type_traits>
#include <vector>
#include <array>
#include <memory>

#include "Xi/Exceptions.hpp"

namespace Xi {

template <typename _ValueT>
class Span {
 public:
  using element_type = _ValueT;
  using value_type = std::remove_cv_t<element_type>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using reference = _ValueT &;
  using const_reference = const value_type &;
  using iterator = pointer;
  using const_iterator = const_pointer;

 private:
  static constexpr const bool is_const_span_v = std::is_const_v<element_type>;
  using data_storage_t = std::conditional_t<is_const_span_v, const_pointer, pointer>;

 private:
  data_storage_t m_data;
  size_type m_size;

 public:
  constexpr Span() noexcept : m_data{nullptr}, m_size{0} {}

  constexpr Span(std::conditional_t<is_const_span_v, const_pointer, pointer> data, size_type size)
      : m_data{data}, m_size{size} {}

  template <std::size_t _N>
  constexpr Span(element_type (&arr)[_N]) noexcept : m_data{std::addressof(arr[0])}, m_size{_N} {}

  template <std::size_t _N>
  constexpr Span(std::array<value_type, _N> &array) : m_data{array.data()}, m_size{_N} {}
  template <std::size_t _N>
  constexpr Span(const std::array<value_type, _N> &array) : m_data{array.data()}, m_size{_N} {}

  constexpr Span(std::vector<value_type> &vector) : m_data{vector.data()}, m_size{vector.size()} {}
  constexpr Span(const std::vector<value_type> &vector) : m_data{vector.data()}, m_size{vector.size()} {}

  size_type size() const noexcept { return this->m_size; }
  size_type size_bytes() const noexcept { return this->m_size * sizeof(value_type); }

  const_pointer data() const { return this->m_data; }
  std::conditional_t<is_const_span_v, const_pointer, pointer> data() { return this->m_data; }

  const_reference operator[](size_type idx) const { return this->m_data[idx]; }
  std::conditional_t<is_const_span_v, const_reference, reference> operator[](size_type idx) {
    return this->m_data[idx];
  }

  operator Span<const value_type>() { return Span<const value_type>{this->m_data, this->m_size}; }

  bool empty() const { return m_size == 0; }

  const_iterator begin() const { return m_data; }
  const_iterator cbegin() const { return m_data; }
  std::conditional_t<is_const_span_v, const_iterator, iterator> begin() { return m_data; }

  const_iterator end() const { return m_data + m_size; }
  const_iterator cend() const { return m_data + m_size; }
  std::conditional_t<is_const_span_v, const_iterator, iterator> end() { return m_data + m_size; }

  Span range(size_type offset, size_type count) const {
    exceptional_if<OutOfRangeError>(offset + count > this->m_size);
    return Span{m_data + offset, count};
  }

  Span range(size_type offset) const {
    exceptional_if<OutOfRangeError>(offset > this->m_size);
    return this->range(offset, this->m_size - offset);
  }

  Span slice(size_type start, size_t end) {
    exceptional_if<OutOfRangeError>(start > this->m_size);
    exceptional_if<OutOfRangeError>(end > this->m_size);
    exceptional_if<OutOfRangeError>(start > end);
    return Span{m_data + start, end - start};
  }

  Span slice(size_type start) {
    exceptional_if<OutOfRangeError>(start > this->m_size);
    return this->slice(start, this->m_size - start);
  }
};

template <typename _ValueT>
using ConstSpan = Span<const _ValueT>;

}  // namespace Xi

#define XI_DECLARE_SPANS(TYPE)         \
  using TYPE##Span = ::Xi::Span<TYPE>; \
  using Const##TYPE##Span = ::Xi::Span<const TYPE>;

#define XI_DECLARE_SPANS_STRUCT(TYPE)  \
  struct TYPE;                         \
  using TYPE##Span = ::Xi::Span<TYPE>; \
  using Const##TYPE##Span = ::Xi::Span<const TYPE>;

#define XI_DECLARE_SPANS_CLASS(TYPE)   \
  class TYPE;                          \
  using TYPE##Span = ::Xi::Span<TYPE>; \
  using Const##TYPE##Span = ::Xi::Span<const TYPE>;
