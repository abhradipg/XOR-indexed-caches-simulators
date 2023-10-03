#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

dataset=$1

echo "Generating Figure 11 from $dataset dataset experiment results..."

echo "Parsing reproduced $dataset dataset experiment results..."

sed -i "s/,/ /g" $artifact_dir/data/reproduced/papi/$dataset-o2.csv

$artifact_dir/scripts/plot-generation/gen-haystack-miss-report.py reproduced $dataset
$artifact_dir/scripts/plot-generation/gen-dinero-miss-report.py reproduced $dataset
$artifact_dir/scripts/plot-generation/gen-papi-miss-report.py reproduced $dataset

$artifact_dir/scripts/plot-generation/gen-plot-figure11.py reproduced $dataset

echo "Done! Figure 11 for $dataset dataset  is available under plots/from-reproduced-data."
