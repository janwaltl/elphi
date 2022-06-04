#!/usr/bin/env bash
# Run clang-tidy on the project.
# $1 = BUILD_DIR.

BUILD_DIR=${1:-build}
SRC_DIR=${PWD}

set -e

mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ${SRC_DIR}
popd

./scripts/filter_libs.sh ${BUILD_DIR}/compile_commands.json

run-clang-tidy -p ${BUILD_DIR}
