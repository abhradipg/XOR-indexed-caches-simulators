#!/usr/bin/env bash

BUILD_DIR=cmake-build-debug
SELF_PATH=$(realpath "$0")
PROJECT_DIR=$(dirname "$SELF_PATH")
mkdir -p $BUILD_DIR
cd $BUILD_DIR || exit
cmake -DCMAKE_BUILD_TYPE=debug "$PROJECT_DIR"
cmake --build . --target warping-cache-simulation -- -j 4
