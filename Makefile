
build-client:
	cmake -B client/build && cmake --build client/build -j $(nproc)

run-client:
	./client/build/client/client

build-server:
	cmake -B server/build && cmake --build server/build -j $(nproc)

run-server:
	./server/build/server/server

all: build-client build-server