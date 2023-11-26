#!/bin/bash

set -e

# internal tests
./build/server/tests/server-tests

echo "Testing server..."

function cleanup {
    sudo kill -9 $pid
}

trap cleanup EXIT
./build/server/src/server &
pid=$!

python3 -m pytest -v ./server/tests/server_tests.py