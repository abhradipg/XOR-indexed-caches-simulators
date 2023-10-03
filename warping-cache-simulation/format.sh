#!/usr/bin/env bash
declare -a dirs=("src"
                 "src/CacheState"
                 "src/Address"
                 "src/IteratorState"
                 "src/ProgramInfo"
                 "src/Settings"
                 "src/Simulation"
                 "src/Snapshot"
                 "src/Util"
                 "src/Warping"
                 "test"
                 "test/CacheState"
                 "test/Address"
                 "test/IteratorState"
                 "test/ProgramInfo"
                 "test/Settings"
                 "test/Simulation"
                 "test/Snapshot"
                 "test/Util"
                 "test/Warping")

function run_clang_format() {
  if [[ $1 == *.cpp ]] || [[ $1 == *.hpp ]]; then
    clang-format -i "$1"
  fi
}

echo "-> Running clang-format..."

for dir in "${dirs[@]}"
do
  for file in "$dir"/*
  do
    run_clang_format "$file"
  done
done

echo "-> Done!"

