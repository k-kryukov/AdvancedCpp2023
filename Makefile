
.PHONY build-client:
	cmake -B client/build && cmake --build client/build -j $(nproc)

run-client:
	./client/build/client/client