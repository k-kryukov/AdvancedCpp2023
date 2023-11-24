#include <gtest/gtest.h>
#include <glog/logging.h>

TEST(Foo, Foo) {
    LOG(INFO) << "Some test!";
}

TEST(Foo, ServiceTests) {
    // Service service;
}

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    testing::InitGoogleTest(&argc, argv);
    FLAGS_alsologtostderr = 1;

    return RUN_ALL_TESTS();
}