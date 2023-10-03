#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

prepare_dataset() {
    local dataset polybench_dataset_dir polybench_header dataset_def scalar_loop_bound_def

    dataset="$1"

    polybench_dataset_dir="$artifact_dir/benchmark/polybench-$dataset"
    polybench_header="$polybench_dataset_dir/utilities/polybench.h"

    dataset_def="# define ${dataset^^}_DATASET // by warping cache simulation"
    scalar_loop_bound_def="# define POLYBENCH_USE_SCALAR_LB 1 // by warping cache simulation"

    our_papi_counters="$artifact_dir/scripts/reproduction/papi_counters.list"

    rm -rf "$polybench_dataset_dir"
    cp -r "$artifact_dir/benchmark/polybench-c-4.2.1-beta" "$polybench_dataset_dir"
    cp "$our_papi_counters" "$polybench_dataset_dir/utilities/papi_counters.list"

    sed -i "31i $dataset_def" "$polybench_header"
    sed -i "31i $scalar_loop_bound_def" "$polybench_header"

    echo "Created $polybench_dataset_dir"
    echo "-> added these to line 31 and 32 in $polybench_header:"
    echo "   $dataset_def"
    echo "   $scalar_loop_bound_def"
    echo "-> copied:"
    echo "   $our_papi_counters"
    echo "   to"
    echo "   $polybench_dataset_dir/utilities/papi_counters.list"
    echo ""
}

prepare_dataset extralarge
prepare_dataset large
prepare_dataset medium
prepare_dataset small

mv $artifact_dir/benchmark/polybench-extralarge $artifact_dir/benchmark/polybench-xlarge
