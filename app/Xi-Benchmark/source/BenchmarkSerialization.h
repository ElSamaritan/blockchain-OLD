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

#include <Serialization/ISerializer.h>

#include <Xi/Utils/ExternalIncludePush.h>
#include <cpuinfo_x86.h>
#include <Xi/Utils/ExternalIncludePop.h>

#include "BenchmarkResult.h"

namespace XiBenchmark {
void serialize_cpu_features(const cpu_features::X86Features& features, CryptoNote::ISerializer& serializer);
void serialize_cpu_info(const cpu_features::X86Info& info, CryptoNote::ISerializer& serializer);
void serialize_benchmark_result(const BenchmarkResult& result, CryptoNote::ISerializer& serializer);
void serialize_benchmark_summary(const BencharkSummary& summary, CryptoNote::ISerializer& serializer);
}  // namespace XiBenchmark
