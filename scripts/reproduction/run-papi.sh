#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

dataset=$1
iterations=$2

bin_dir=$artifact_dir/benchmark/papi-binaries/$dataset
out_dir=$artifact_dir/data/reproduced/papi
mkdir -p $out_dir

out_file=$out_dir/$dataset.csv
printf "KERNEL L1_MISS L2_MISS\n" > "$out_file"

echo "Running PAPI binaries in $bin_dir..."
for bin in "$bin_dir"/*
do
  kernel_name=$(basename $bin)
  printf "-> %s.." "$kernel_name"
  for i in $(seq 10);
  do
    printf ".$i"
    out=$($bin)
    printf "%s %s\n" "$kernel_name" "$out" >> "$out_file"
  done
  printf "\n"
done

