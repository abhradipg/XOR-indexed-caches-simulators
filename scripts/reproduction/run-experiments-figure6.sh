#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

dataset=$1
repdir=$artifact_dir/scripts/reproduction

$repdir/run-polybench-all.sh $dataset true true $repdir/cache-configs/lru-cascadelake-l1.config
$repdir/run-polybench-all.sh $dataset true true $repdir/cache-configs/cascadelake-l1.config
$repdir/run-polybench-all.sh $dataset true true $repdir/cache-configs/qlru-cascadelake-l1.config
$repdir/run-polybench-all.sh $dataset true true $repdir/cache-configs/fifo-cascadelake-l1.config
$repdir/run-polybench-all.sh $dataset false false $repdir/cache-configs/lru-cascadelake-l1.config
$repdir/run-polybench-all.sh $dataset false false $repdir/cache-configs/cascadelake-l1.config
$repdir/run-polybench-all.sh $dataset false false $repdir/cache-configs/qlru-cascadelake-l1.config
$repdir/run-polybench-all.sh $dataset false false $repdir/cache-configs/fifo-cascadelake-l1.config
