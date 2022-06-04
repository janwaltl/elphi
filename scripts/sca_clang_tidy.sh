#!/usr/bin/env bash
# Run clang-tidy on the project.
# Meant to be run from project root.

BUILD_DIR=build

mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
popd

./scripts/filter_libs.sh ${BUILD_DIR}/compile_commands.json

run-clang-tidy -p ${BUILD_DIR}
