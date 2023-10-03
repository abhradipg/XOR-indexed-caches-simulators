#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

dataset=$1
repdir=$artifact_dir/scripts/reproduction
resdir=$artifact_dir/data/reproduced

$repdir/run-polybench-all.sh $dataset true true $repdir/cache-configs/lru-cascadelake-l1.config
$repdir/run-polybench-all.sh $dataset true true $repdir/cache-configs/cascadelake-l1.config
$repdir/run-polybench-all.sh $dataset true true $repdir/cache-configs/qlru-cascadelake-l1.config
$repdir/run-polybench-all.sh $dataset true true $repdir/cache-configs/fifo-cascadelake-l1.config
$repdir/run-polybench-all.sh $dataset true true $repdir/cache-configs/fully-assoc-lru-cascadelake-l1.config
$repdir/create-csv-reports.sh
$repdir/rename-copy-csv.sh $dataset true true lru-cascadelake-l1 miss "warping-$dataset-lru-l1.csv"
$repdir/rename-copy-csv.sh $dataset true true cascadelake-l1 miss "warping-$dataset-plru-l1.csv"
$repdir/rename-copy-csv.sh $dataset true true qlru-cascadelake-l1 miss "warping-$dataset-qlru-l1.csv"
$repdir/rename-copy-csv.sh $dataset true true fifo-cascadelake-l1 miss "warping-$dataset-fifo-l1.csv"
$repdir/rename-copy-csv.sh $dataset true true fully-assoc-lru-cascadelake-l1 miss "warping-$dataset-full-assoc-lru-l1.csv"
