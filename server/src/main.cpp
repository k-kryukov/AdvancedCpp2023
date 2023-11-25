#include <glog/logging.h>

#include "Server.hpp"

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = 1;

	Server server{12345};
	server.start();

    return 0;
}