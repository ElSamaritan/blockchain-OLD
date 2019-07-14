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

/*!
 * Include this header before you include external headers yielding warnings.
 *
 * Note: You must include the corresponding Pop header once you included all external headers.
 */

#include <leathers/push>
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"
#endif
#include <leathers/all>
#if defined(_MSC_VER)
#pragma warning(disable : 4003)
#pragma warning(disable : 4067)
#pragma warning(disable : 4267)
#pragma warning(disable : 4324)
#pragma warning(disable : 4456)
#pragma warning(disable : 4458)
#pragma warning(disable : 4702)
#pragma warning(disable : 4706)
#define NOGDI
#endif
