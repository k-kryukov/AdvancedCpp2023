#include <gtest/gtest.h>
#include <glog/logging.h>

#include "Service.hpp"

TEST(Foo, Foo) {
    LOG(INFO) << "Some test!";
}

TEST(Foo, ServiceUsersTests) {
	Service service;
	auto users = std::vector{"@krecov", "username"};

    std::string pswdHash = "11111111111111111111";
	service.createUser(users.at(0), pswdHash);
	service.createUser(users.at(1), pswdHash);

    std::string wrongPassword = "11111111111211111111";
    ASSERT_THROW(
        service.removeUser(users.at(0), wrongPassword),
        std::runtime_error
    );

    ASSERT_EQ(service.getUsers().size(), 2);
    service.removeUser(users.at(0), pswdHash);
    service.removeUser(users.at(1), pswdHash);
    ASSERT_EQ(service.getUsers().size(), 0);
}

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    testing::InitGoogleTest(&argc, argv);
    FLAGS_alsologtostderr = 1;

    return RUN_ALL_TESTS();
}