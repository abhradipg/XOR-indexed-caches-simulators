#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

dataset=$1
warp=$2
heuristics=$3
cache_config=$4
cache_config_name=$(basename "$cache_config")
cache_config_name=${cache_config_name%.*}

source $artifact_dir/scripts/reproduction/polybench-kernels.config

polybench_dir="$artifact_dir/benchmark/polybench-$dataset"
polybench_include=$polybench_dir/utilities
sys_include=/usr/local/include,/usr/include
bin=$artifact_dir/cmake-build-release/src/warping-cache-simulation

reproduced_dir="$artifact_dir/data/reproduced"
json_dir="$reproduced_dir/json/$dataset"
mkdir -p "$json_dir"
out_file="$json_dir/$warp-$heuristics-$cache_config_name.json"
printf "" > "$out_file"

is_kernel_disabled() {
  echo ${disabled_polybench_src_list[@]} | tr " " '\n' | grep -F -q -x "$1"
}

echo_includes_for_polybench_src() {
  local polybench_src
  polybench_src=$1
  polybench_src_dir=$(dirname polybench_src)
  echo "$sys_include,$polybench_include,$polybench_dir/$polybench_src_dir"
}

run_simulation_for_polybench_src() {
  local polybench_src src includes name
  polybench_src=$1
  src=$polybench_dir/$polybench_src
  includes=$(echo_includes_for_polybench_src "$polybench_src")
  name=$(basename "${polybench_src%.c}")
  printf "\"%s\": " "$name" >> "$out_file"
  echo "-> $polybench_src..."
  $bin "$src" "$cache_config" \
                -w "$warp" \
                -h "$heuristics" \
                -i "$includes" >> "$out_file"
  printf ",\n" >> "$out_file"
}

run_simulation_for_all_polybench_srcs() {
  printf "{\n" >> "$out_file"
  echo "Simulating all examples in $(basename "$polybench_dir")..."
  echo "Settings warp = $warp, heuristics = $heuristics,"\
       "cache_config = $cache_config"
  for polybench_src in "${polybench_src_list[@]}"
  do
    if is_kernel_disabled "$polybench_src"; then
      echo "-> [skip-disabled] $polybench_src"
    else
      run_simulation_for_polybench_src "$polybench_src"
    fi
  done

  sed -i '$ d' "$out_file"
  printf "}\n" >> "$out_file"
}

run_simulation_for_all_polybench_srcs

echo "Done, benchmark json result at $out_file!"
echo ""
