all:
	rm -rf build && cmake -B build && cmake --build build -j $(nproc)

format:
	/bin/bash format.bash

test-server:
	/bin/bash test.bash --server

test-client:
	/bin/bash test.bash --client