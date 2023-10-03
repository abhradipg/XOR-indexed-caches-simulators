#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

dataset=$1

$artifact_dir/scripts/reproduction/run-experiments-figure8.sh $dataset

echo "Parsing reproduced HayStack $dataset dataset experiment results..."

$artifact_dir/scripts/plot-generation/gen-haystack-duration-report.py reproduced $dataset

echo "Generating Figure 8 from $dataset dataset experiment results..."

$artifact_dir/scripts/plot-generation/gen-plot-figure8.py reproduced $dataset

echo "Done! Figure 8 for $dataset dataset is available under plots/from-reproduced-data."
