all:
	rm -rf build && cmake -B build && cmake --build build -j $(nproc)

format:
	/bin/bash format.bash