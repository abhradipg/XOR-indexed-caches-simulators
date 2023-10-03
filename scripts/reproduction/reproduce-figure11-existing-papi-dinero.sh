#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

dataset=$1

$artifact_dir/scripts/reproduction/run-experiments-figure11-existing-papi-dinero.sh $dataset

echo "Generating Figure 11 from $dataset dataset experiment results..."
echo "Using existing data for PAPI-C and DineroIV results..."

echo "Parsing reproduced HayStack $dataset dataset experiment results..."

$artifact_dir/scripts/plot-generation/gen-haystack-miss-report.py reproduced $dataset

$artifact_dir/scripts/plot-generation/gen-plot-figure11-existing-papi-dinero.py reproduced $dataset

echo "Done! Figure 11 for $dataset dataset (from existing PAPI and DineroIV data) is available under plots/from-reproduced-data."
