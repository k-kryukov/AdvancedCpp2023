#include <glog/logging.h>

#include "Server.hpp"

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = 1;

    try {
        Server server{12345};
        server.start();
        server.start_ctx();
    } catch (std::exception const& ex) {
        LOG(FATAL) << "Error happened during server starting: " << std::move(ex.what());
    }

    return 0;
}