#!/usr/bin/env bash

artifact_dir=$(realpath $(dirname "$0")/../..)

dataset1=$1
dataset2=$2
repdir=$artifact_dir/scripts/reproduction

$repdir/run-polybench-all.sh $dataset1 true true $repdir/cache-configs/cascadelake-l1.config
$repdir/run-polybench-all.sh $dataset2 true true $repdir/cache-configs/cascadelake-l1.config
$repdir/run-polybench-all.sh $dataset1 false false $repdir/cache-configs/cascadelake-l1.config
$repdir/run-polybench-all.sh $dataset2 false false $repdir/cache-configs/cascadelake-l1.config

$repdir/create-csv-reports.sh

$repdir/rename-copy-csv.sh $dataset1 true true cascadelake-l1 duration "warping-$dataset1-plru-l1.csv"
$repdir/rename-copy-csv.sh $dataset2 true true cascadelake-l1 duration "warping-$dataset2-plru-l1.csv"
$repdir/rename-copy-csv.sh $dataset1 false false cascadelake-l1 duration "nonwarping-$dataset1-plru-l1.csv"
$repdir/rename-copy-csv.sh $dataset2 false false cascadelake-l1 duration "nonwarping-$dataset2-plru-l1.csv"
