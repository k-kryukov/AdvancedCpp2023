all:
	rm -rf build && cmake -B build && cmake --build build -j $(nproc)

format:
	/bin/bash format.bash

test-server:
	/bin/bash test.bash --server

test-client:
	/bin/bash test.bash --client

test: test-server test-client

clean:
	rm -rf build .pytest_cache cmake-build-debug