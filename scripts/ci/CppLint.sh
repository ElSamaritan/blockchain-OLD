#!/bin/bash

set +e
cpplint                                                     \
  --exclude=external/*                                      \
  --output=junit                                            \
  --recursive ./src/                                        \
2> cpplint.xml

set -e
if [ ! -d ./.site/CppLint ]; then
  mkdir -p ./.site/CppLint
fi
junit2html cpplint.xml ./.site/CppLint/index.html
