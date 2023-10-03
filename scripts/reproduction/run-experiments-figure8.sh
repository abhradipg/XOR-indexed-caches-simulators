#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

dataset=$1
repdir=$artifact_dir/scripts/reproduction
resdir=$artifact_dir/data/reproduced

$repdir/run-polybench-all.sh $dataset true true $repdir/cache-configs/fully-assoc-lru-cascadelake-l1.config
$repdir/create-csv-reports.sh
$repdir/rename-copy-csv.sh $dataset true true fully-assoc-lru-cascadelake-l1 duration \
    "warping-$dataset-full-assoc-lru-l1.csv"
