#!/usr/bin/env bash
# Build and run unit tests with release build, save junit reports.
# $1 = BUILD_DIR.

BUILD_DIR=${1:-build/junit}
SRC_DIR=${PWD}

NORMAL='\033[0m'
OK='\033[0;32m'
ERROR='\033[0;31m'
INFO='\033[0;36m'

set -e

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

cmake -DCMAKE_BUILD_TYPE=Release -DJUNIT_TEST_OUTPUT=ON ${SRC_DIR} -DCMAKE_CXX_COMPILER=g++-10

# Build project and tests
make
if [ $? -ne 0 ]; then 
	echo -e "${ERROR}FAILED TO BUILD THE PROJECT${NORMAL}"
	exit 1
fi

# Run the test suite
ctest -T Test
