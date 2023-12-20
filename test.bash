#!/bin/bash

set -e

type=$1
if [[ $1 == "--client" ]]; then
    function cleanup {
        sudo kill -9 $pid
    }

    trap cleanup EXIT
    # ./build/client/tests/almost_echo_server &
    ./build/server/src/server &
    pid=$!

    ./build/client/tests/client-tests

    echo "Testing client..."

    # python3 -m pytest -v ./client/tests/test_almost_echo_server.py
elif [[ $1 == "--server" ]]; then
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
else
    echo "Unknown option, supported are --server/--test"
fi