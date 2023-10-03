#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

dataset1=$1
dataset2=$2

$artifact_dir/scripts/reproduction/run-experiments-figure7.sh $dataset1 $dataset2

echo "Generating Figure 7 from $dataset1 and $dataset2 dataset experiment results..."

$artifact_dir/scripts/plot-generation/gen-plot-figure7.py reproduced $dataset1 $dataset2

echo "Done! Figure 7 for $dataset1 vs $dataset2 dataset is available under plots/from-reproduced-data."
