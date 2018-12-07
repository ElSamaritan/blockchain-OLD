#!/bin/bash
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
