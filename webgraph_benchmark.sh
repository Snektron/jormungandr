#!/usr/bin/bash

set -eu

ROOT=$(realpath $(dirname $0))

gradle -b "$ROOT/benchmark/build.gradle" install
"$ROOT/benchmark.sh" --threaded "$ROOT/benchmark/build/install/benchmark/bin/benchmark"
