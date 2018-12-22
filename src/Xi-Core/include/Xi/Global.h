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

namespace Xi {
/*!
 * \brief Unreferenced marks the arguments as intentionally unreferenced.
 */
template <typename... Ts>
inline void Unreferenced(Ts&&...) {}
}  // namespace Xi

/*!
 * \def XI_UNUSED Marks a variable unused intentionally.
 */
#define XI_UNUSED(...) \
  { (decltype(Xi::Unreferenced(__VA_ARGS__)))0; }

/*!
 * \def XI_DELETE_COPY(CLASS_NAME) deletes any possible default generated copy constructor/assignment for CLASS_NAME
 */
#define XI_DELETE_COPY(CLASS_NAME)        \
  CLASS_NAME(const CLASS_NAME&) = delete; \
  CLASS_NAME& operator=(const CLASS_NAME&) = delete

/*!
 * \def XI_DEFAULT_COPY(CLASS_NAME) implements a default copy constructor/assignment for CLASS_NAME
 */
#define XI_DEFAULT_COPY(CLASS_NAME)        \
  CLASS_NAME(const CLASS_NAME&) = default; \
  CLASS_NAME& operator=(const CLASS_NAME&) = default

/*!
 * \def XI_DELETE_MOVE(CLASS_NAME) deletes any possible default generated move constructor/assignment for CLASS_NAME
 */
#define XI_DELETE_MOVE(CLASS_NAME)   \
  CLASS_NAME(CLASS_NAME&&) = delete; \
  CLASS_NAME& operator=(CLASS_NAME&&) = delete

/*!
 * \def XI_DEFAULT_MOVE(CLASS_NAME) implements a default move constructor/assignment for CLASS_NAME
 */
#define XI_DEFAULT_MOVE(CLASS_NAME)   \
  CLASS_NAME(CLASS_NAME&&) = default; \
  CLASS_NAME& operator=(CLASS_NAME&&) = default
