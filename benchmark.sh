#!/usr/bin/bash

set -eu

ROOT=$(realpath $(dirname $0))
DATASET_DIR=$(realpath $ROOT/test)
DATASETS="$(find $DATASET_DIR -type f -name *.graph)"
BENCHMARK=""
ENABLE_THREADED=0
REPETITIONS=5

while (( "$#" )); do
    case "$1" in
        --threaded)
            ENABLE_THREADED=1
            shift
            ;;
        -*)
            echo "Error: Unknown flag $1"
            exit 1
            ;;
        *)
            if [ -n "$BENCHMARK" ]; then
                echo "Error: Unknown parameter $1"
                exit 1
            fi

            BENCHMARK=$1
            shift
        ;;
    esac
done

if [ -z "$BENCHMARK" ]; then
    echo "Error: Missing argument <benchmark executable>"
    exit 1
fi

function run_tests() {
    for F in $DATASETS; do
        GRAPH="$(basename $F .graph)"
        GRAPH_PATH="$(dirname $F)/$GRAPH"
        RUNTIMES=""
        for I in $(seq $REPETITIONS); do
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

echo "Running decode tests"
run_tests run_decode_test

echo "Running encode tests"
run_tests run_encode_test

if [ "$ENABLE_THREADED" == "1" ]; then
    echo "Running threaded encode tests"
    run_tests run_encode_test_threaded
fi
