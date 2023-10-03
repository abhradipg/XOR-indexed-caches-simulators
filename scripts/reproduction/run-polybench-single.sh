#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

kernel_name=$1
dataset=$2
warp=$3
heuristics=$4
cache_config=$5
cache_config_name=$(basename "$cache_config")
cache_config_name=${cache_config_name%.*}

polybench_dir="$artifact_dir/benchmark/polybench-$dataset"
polybench_include=$polybench_dir/utilities
sys_include=/usr/local/include,/usr/include
bin=$artifact_dir/cmake-build-release/src/warping-cache-simulation

src=$(find "$polybench_dir" -name "$kernel_name")/$kernel_name.c
echo "Found kernel $kernel_name at $src!"

includes=$sys_include,$polybench_include,$(dirname src)

echo "Simulating $src with $dataset dataset..."
echo "warp = $warp, heuristics = $heuristics, cache_config = $cache_config"

echo "-> $kernel_name..."
./$bin "$src" "$cache_config" \
              -w "$warp" \
              -h "$heuristics" \
              -i "$includes"
