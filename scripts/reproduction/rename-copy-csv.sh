#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

repdir=$artifact_dir/scripts/reproduction
resdir=$artifact_dir/data/reproduced

dataset=$1
warp=$2
heuristics=$3
cache=$4
type=$5 # duration/miss
new_name=$6

csv_file="$resdir/csv/$dataset/$warp-$heuristics-$cache-$type.csv"
csv_file_copy="$resdir/$type/$new_name"

mkdir -p "$resdir/$type"
echo "Copying"
echo "  -> source: $csv_file"
echo "  -> dest  : $csv_file_copy"
cp "$csv_file" "$csv_file_copy"
