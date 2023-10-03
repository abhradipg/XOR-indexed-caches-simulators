#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

echo "Generating plots from existing data..."

echo "-> Figure 6..."
$artifact_dir/scripts/plot-generation/gen-plot-figure6.py existing large

echo "-> Figure 7..."
$artifact_dir/scripts/plot-generation/gen-plot-figure7.py existing large xlarge

echo "-> Figure 8..."
$artifact_dir/scripts/plot-generation/gen-plot-figure8.py existing large

echo "-> Figure 9..."
$artifact_dir/scripts/plot-generation/gen-plot-figure9.py existing large

echo "-> Figure 10..."
$artifact_dir/scripts/plot-generation/gen-plot-figure10.py existing large

echo "-> Figure 11..."
$artifact_dir/scripts/plot-generation/gen-plot-figure11.py existing large
