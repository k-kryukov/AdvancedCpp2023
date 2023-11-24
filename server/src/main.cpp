#include <iostream>

#include <glog/logging.h>

#include "Service.hpp"

void setupLogging(char* argv[]) {
	google::InitGoogleLogging(argv[0]);
	FLAGS_alsologtostderr = 1;
}

int main(int argc, char *argv[]) {
	setupLogging(argv);
	LOG(INFO) << "Welcome to my server!";

	Service service;
	// service.createUser("Kostya", )

	return 0;
}
