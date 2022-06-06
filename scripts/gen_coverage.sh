#!/usr/bin/env bash
# Generate code coverage
# $1 = BUILD_DIR.

BUILD_DIR=${1:-build}
SRC_DIR=${PWD}

NORMAL='\033[0m'
OK='\033[0;32m'
ERROR='\033[0;31m'
INFO='\033[0;36m'
BAR=================================================================================


mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON  -DCMAKE_CXX_COMPILER=g++-10 ${SRC_DIR}

# Build project and tests
make
if [ $? -ne 0 ]; then 
	echo -e "${ERROR}FAILED TO BUILD THE PROJECT${NORMAL}"
	exit 1
fi
# Clean previous reports
rm -rf reports/*

# Run the test suite
make coverage
result=$?

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
else
	echo -e "${ERROR}$BAR"
	echo -e "SOME TEST SCENARIOS FAILED."
	echo -e "${BAR}${NORMAL}"
fi

echo -e "${OK}$BAR"
echo -e "COVERAGE REPORT IS IN ${BUILD_DIR}/coverage/index.html"
echo -e "${BAR}${NORMAL}"
exit ${result}
