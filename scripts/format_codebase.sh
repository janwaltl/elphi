#!/usr/bin/env bash
# Reformat codebase.

NORMAL='\033[0m'
OK='\033[0;32m'
ERROR='\033[0;31m'
INFO='\033[0;36m'

git ls-files '*.hpp' '*.cpp' | xargs clang-format -i --verbose --style=file

echo -e "${INFO}FILES REFORMATTED.${NORMAL}"
