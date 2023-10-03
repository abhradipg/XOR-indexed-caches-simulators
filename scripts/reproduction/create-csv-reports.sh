#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

declare -a json_dirs=("$artifact_dir/data/reproduced/json/small"
                      "$artifact_dir/data/reproduced/json/medium"
                      "$artifact_dir/data/reproduced/json/large"
                      "$artifact_dir/data/reproduced/json/xlarge")

function create_csv_report_from_json_result() {
  if [[ $1 == *.json ]]; then
    $artifact_dir/scripts/reproduction/convert-json-to-miss-csv.py $1
    $artifact_dir/scripts/reproduction/convert-json-to-duration-csv.py $1
  fi
}

echo "Creating csv reports from all reproduced json results so far..."

for dir in "${json_dirs[@]}"
do
  mkdir -p "${dir//json/csv}"
  for file in "$dir"/*
  do
    create_csv_report_from_json_result "$file"
  done
done

echo "Done!"
echo ""
