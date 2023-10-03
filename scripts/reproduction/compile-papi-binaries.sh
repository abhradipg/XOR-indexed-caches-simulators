#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

dataset=$1
polybench_dir="$artifact_dir/benchmark/polybench-$dataset"
polybench_src=$polybench_dir/utilities/polybench.c
polybench_include=$polybench_dir/utilities

bin_folder="$artifact_dir/benchmark/papi-binaries/$dataset"
mkdir -p "$bin_folder"

source $artifact_dir/scripts/reproduction/polybench-kernels.config

compile_kernel() {
  local kernel_src kernel_include kernel_name kernel_bin
  kernel_src="$polybench_dir/$1"
  kernel_include="$(dirname "$kernel_src")"
  kernel_name="$(basename "${kernel_src%.c}")"
  kernel_bin=$bin_folder/$kernel_name
  gcc -O2 \
      -I "$polybench_include" -I "$kernel_include" \
      -I /usr/local/include -I /usr/include \
      "$polybench_src" "$kernel_src" \
      -lm -DPOLYBENCH_PAPI -lpapi \
      -o "$kernel_bin"
}

compile_all_kernels() {
  echo "Compiling PAPI binaries from $polybench_dir..."
  for kernel_src in "${polybench_src_list[@]}"
  do
    echo "-> $kernel_src..."
    compile_kernel "$kernel_src"
  done
}

compile_all_kernels
