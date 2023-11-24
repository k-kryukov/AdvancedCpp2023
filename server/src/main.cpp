#include <iostream>

#include <glog/logging.h>

#include "Service.hpp"

void setupLogging(char* argv[]) {
	google::InitGoogleLogging(argv[0]);
	FLAGS_alsologtostderr = 1;

	LOG(INFO) << "Welcome to my server!";
}

int main(int argc, char *argv[]) {
	setupLogging(argv);

	return 0;
}
