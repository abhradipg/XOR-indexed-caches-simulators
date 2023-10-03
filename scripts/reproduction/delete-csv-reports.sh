#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

declare -a csv_dirs=("$artifact_dir/data/reproduced/csv/small"
                     "$artifact_dir/data/reproduced/csv/medium"
                     "$artifact_dir/data/reproduced/csv/large"
                     "$artifact_dir/data/reproduced/csv/xlarge")

echo "-> Deleting all existing csv reports..."

for dir in "${csv_dirs[@]}"
do
  for file in "$dir"/*
  do
    if [[ $file == *.csv ]]; then
        rm -rf $file
    fi
  done
done

echo "-> Done!"
