#include <gtest/gtest.h>
#include <glog/logging.h>

#include "Service.hpp"

TEST(Foo, Foo) {
    LOG(INFO) << "Some test!";
}

TEST(Foo, ServiceUsersTest) {
	Service service;
	auto users = std::vector<std::string>{"@krecov", "username"};
    std::sort(users.begin(), users.end());

    std::string pswdHash = "11111111111111111111";
    for (auto&& user : users)
	    service.createUser(user, pswdHash);

    auto extractedUsers = service.getUsers();
    std::sort(extractedUsers.begin(), extractedUsers.end());
    ASSERT_TRUE(users == extractedUsers);
    ASSERT_EQ(service.getUsers().size(), 2);

    ASSERT_THROW(
        service.removeUser(users.at(0), pswdHash + "0"),
        std::runtime_error
    );

    ASSERT_EQ(service.getUsers().size(), 2);

    service.removeUser(users.at(0), pswdHash);
    service.removeUser(users.at(1), pswdHash);
    ASSERT_EQ(service.getUsers().size(), 0);
}

TEST(Foo, NotesTest) {
	Service service;
	auto users = std::vector<std::string>{"@krecov", "k.kryukov"};

    std::string pswdHash = "11111111111111111111";
    for (auto&& user : users) {
	    service.createUser(user, pswdHash);
        service.addNote(user, pswdHash, std::make_shared<Note>("Hello!"));
    }
    service.addNote(users.back(), pswdHash, std::make_shared<Note>("Bye!"));

    for (auto&& user : users) {
        auto notes = service.getNotesList(user);
        ASSERT_EQ(notes.size(), (user != users.back()) ? 1 : 2);
    }

    ASSERT_THROW(
        service.removeNote(users.back(), 2),
        std::invalid_argument
    );
    service.removeNote(users.back(), 1);
    service.removeNote(users.back(), 0);
    ASSERT_THROW(
        service.removeNote(users.back(), 0),
        std::invalid_argument
    );

    auto notes = service.getNotesList(users.back());
    ASSERT_EQ(notes.size(), 0);
}

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    testing::InitGoogleTest(&argc, argv);
    FLAGS_alsologtostderr = 1;

    return RUN_ALL_TESTS();
}