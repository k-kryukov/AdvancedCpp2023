#include <gtest/gtest.h>
#include <glog/logging.h>

#include "Service.hpp"

TEST(Foo, Foo) {
    LOG(INFO) << "Some test!";
}

TEST(Foo, ServiceTests) {
	Service service;
	std::string userName = "Kostya";
    auto pswdHash = 123456;
	service.createUser(userName, pswdHash);
	LOG(INFO) << "New note with ID " << service.addNote(userName, std::make_shared<Note>("Some important notes"));
	LOG(INFO) << "New note with ID " << service.addNote(userName, std::make_shared<Note>("Some important notes x2"));

	LOG(INFO) << "Registered notes:";
	for (auto x : service.getNotesList(userName))
		LOG(INFO) << x->getText();

	service.removeNote(userName, 0);

	for (auto x : service.getNotesList(userName))
		LOG(INFO) << x->getText();

    ASSERT_THROW(
        service.removeUser(userName, pswdHash - 1),
        std::runtime_error
    );

    service.removeUser(userName, pswdHash);
}

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    testing::InitGoogleTest(&argc, argv);
    FLAGS_alsologtostderr = 1;

    return RUN_ALL_TESTS();
}