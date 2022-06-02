#!/usr/bin/env bash
# Filter commands used to build external dependencies.
# $1 - path to compile_commands.json

cat $1 | jq '. - map(select(.directory | contains("libs")))' > tmp.json
mv tmp.json $1
