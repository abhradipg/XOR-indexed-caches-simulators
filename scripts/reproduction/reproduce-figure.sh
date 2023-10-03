#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

figure_number=$1
dataset=$2

$artifact_dir/scripts/reproduction/run-experiments-figure$figure_number.sh $dataset

echo "Generating Figure $figure_number from $dataset dataset experiment results..."

$artifact_dir/scripts/plot-generation/gen-plot-figure$figure_number.py reproduced $dataset

echo "Done! Figure $figure_number for $dataset dataset is available under plots/from-reproduced-data."
