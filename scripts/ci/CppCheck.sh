#!/bin/bash

# ============================================================================================== #
#                                                                                                #
#                                       Xi Blockchain                                            #
#                                                                                                #
# ---------------------------------------------------------------------------------------------- #
# This file is part of the Galaxia Project - Xi Blockchain                                       #
# ---------------------------------------------------------------------------------------------- #
#                                                                                                #
# Copyright 2018-2019 Galaxia Project Developers                                                 #
#                                                                                                #
# This program is free software: you can redistribute it and/or modify it under the terms of the #
# GNU General Public License as published by the Free Software Foundation, either version 3 of   #
# the License, or (at your option) any later version.                                            #
#                                                                                                #
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      #
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      #
# See the GNU General Public License for more details.                                           #
#                                                                                                #
# You should have received a copy of the GNU General Public License along with this program.     #
# If not, see <https://www.gnu.org/licenses/>.                                                   #
#                                                                                                #
# ============================================================================================== #

set -e

if [ ! -d ./.site/CppCheck ]; then
  mkdir -p ./.site/CppCheck
fi

if [ ! -d ./.build ]; then
  mkdir -p ./.build
fi

pushd ./.build

cmake                                 \
  -DCMAKE_BUILD_TYPE=Release          \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON  \
  -DXI_BUILD_TESTSUITE=ON             \
  ..

cppcheck                                        \
  --enable=all                                  \
  --xml --xml-version=2                         \
  --inline-suppr                                \
  --project=compile_commands.json               \
  -i$CI_PROJECT_DIR/src/crypto/cnx/operations/  \
  -i$CI_PROJECT_DIR/external/                   \
  $CI_PROJECT_DIR/src                           \
2> ../cppcheck.xml

popd

python ./scripts/ci/cppcheck-htmlreport.py  \
  --title="Xi - CppCheck"                   \
  --file=./cppcheck.xml                     \
  --report-dir=./.site/CppCheck             \
  --source-encoding=utf8                    \
  --source-dir=$CI_PROJECT_DIR
