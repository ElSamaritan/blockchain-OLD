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

#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(_STD_VERSION) && __STDC_VERSION__ >= 201112 && !defined __STDC_NO_THREADS__
#define XI_RUNTIME_THREAD_LOCAL _Thread_local
#elif defined _WIN32 && (defined _MSC_VER || defined __ICL || defined __DMC__ || defined __BORLANDC__)
#define XI_RUNTIME_THREAD_LOCAL __declspec(thread)
#elif defined __GNUC__ || defined __SUNPRO_C || defined __xlC__
#define XI_RUNTIME_THREAD_LOCAL __thread
#endif

#if defined(__cplusplus)
}
#endif
