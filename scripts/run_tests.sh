#!/usr/bin/env bash
# Build and run unit tests with release build, show results.
# $1 = BUILD_DIR.

BUILD_DIR=${1:-build/release}
SRC_DIR=${PWD}

NORMAL='\033[0m'
OK='\033[0;32m'
ERROR='\033[0;31m'
INFO='\033[0;36m'
BAR=================================================================================

cd ${BUILD_DIR}

# Build project and tests
make
if [ $? -ne 0 ]; then 
	echo -e "${ERROR}FAILED TO BUILD THE PROJECT${NORMAL}"
	exit 1
fi

# Run the test suite
ctest -T Test
result=$?

# Print all outputs from Catch2 directly
# default ctest is just meh and verbose misses colours.

for file in "reports/*.txt"; do
	echo -e "${INFO}$BAR"
	echo -e "REPORT: ${file} "
	echo -e "${BAR}${NORMAL}"
	cat $file
	echo -e "${INFO}$BAR${NORMAL}"
done
if [ ${result} -eq 0 ]; then
	echo -e "${OK}$BAR"
	echo -e "ALL TESTS SCENARIOS PASSED."
	echo -e "${BAR}${NORMAL}"
	exit 0
else
	echo -e "${ERROR}$BAR"
	echo -e "SOME TEST SCENARIOS FAILED."
	echo -e "${BAR}${NORMAL}"
	exit 1
fi
