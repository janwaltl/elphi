#!/usr/bin/env bash
# Generate code coverage
# $1 = BUILD_DIR.

BUILD_DIR=${1:-build}
SRC_DIR=${PWD}

NORMAL='\033[0m'
OK='\033[0;32m'
ERROR='\033[0;31m'
INFO='\033[0;36m'

set -e

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ${SRC_DIR}

# Build project and tests
make
if [ $? -ne 0 ]; then 
	echo -e "${ERROR}FAILED TO BUILD THE PROJECT${NORMAL}"
	exit 1
fi

# Run the test suite
make coverage
