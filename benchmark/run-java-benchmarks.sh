#/usr/bin/bash

set -eu

ROOT=$(realpath $(dirname $0))
DATASET_DIR=$(realpath $ROOT/../test)
DATASETS="$(find $DATASET_DIR -type f -name *.graph)"
BENCHMARK="$ROOT/build/install/benchmark/bin/benchmark"
REPETITIONS=3

function run_tests() {
    for F in $DATASETS; do
        GRAPH="$(basename $F .graph)"
        GRAPH_PATH="$(dirname $F)/$GRAPH"
        RUNTIMES=""
        for I in $(seq $REPETITIONS); do
            # RUNTIME="$1 $GRAPH_PATH"
            # RUNTIME="$($BENCHMARK decode $GRAPH_PATH)"
            $1 $GRAPH_PATH
            RUNTIMES="$RUNTIMES $RUNTIME"
        done
        echo $GRAPH $RUNTIMES
    done
}

function run_decode_test() {
    RUNTIME=$($BENCHMARK decode $1)
}

function run_encode_test() {
    RUNTIME=$($BENCHMARK encode $1 $1.out)
    rm $1.out.*
}

function run_encode_test_threaded() {
    RUNTIME=$($BENCHMARK encode --threads 16 $1 $1.out)
    rm $1.out.*
}

# echo "Running decode tests"
# run_tests run_decode_test

# echo "Running encode tests"
# run_tests run_encode_test

echo "Running threaded encode tests"
run_tests run_encode_test_threaded
