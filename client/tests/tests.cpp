#include <glog/logging.h>
#include <gtest/gtest.h>

#include <QApplication>
#include <boost/asio.hpp>

#include "Service.hpp"

TEST(ServiceTests, TestConnectorUsersManager) {
  Connector conn;
  int res;

  // check + create + check
  ASSERT_EQ(conn.checkCreds("krecov", "123"), 404);
  ASSERT_EQ(conn.createNewUser("krecov", "123"), 200);
  ASSERT_EQ(conn.checkCreds("krecov", "123"), 200);

  // create existing user + existing with different password
  ASSERT_EQ(conn.createNewUser("krecov", "123"), 400);
  ASSERT_EQ(conn.createNewUser("krecov", "1234"), 400);

  // add + check + remove + check
  ASSERT_EQ(conn.createNewUser("krecov1", "123"), 200);
  ASSERT_EQ(conn.checkCreds("krecov1", "1234"), 404);
  ASSERT_EQ(conn.removeUser("krecov1", "123"), 200);
  ASSERT_EQ(conn.checkCreds("krecov1", "123"), 404);

  // remove with wrong password + check + remove + check
  ASSERT_EQ(conn.removeUser("krecov", "1234"), 400);
  ASSERT_EQ(conn.checkCreds("krecov", "123"), 200);
  ASSERT_EQ(conn.removeUser("krecov", "123"), 200);
  ASSERT_EQ(conn.checkCreds("krecov", "123"), 404);

  ASSERT_EQ(conn.getUsers().empty(), true);
}

TEST(ServiceTests, TestConnectorNotesManager) {
  Connector conn;
  auto [username, password] = std::tuple("krecov", "123");

  // create + check
  ASSERT_EQ(conn.createNewUser(username, password), 200);
  ASSERT_EQ(conn.checkCreds(username, password), 200);

  // create N times + check notes cnt
  ASSERT_EQ(conn.createNote(username, password, "Note1"), 200);
  ASSERT_EQ(conn.createNote(username, password, "Note2"), 200);
  ASSERT_EQ(conn.createNote(username, password, "Note1"), 200);
  ASSERT_EQ(conn.createNote(username, password, "AgainNote1"), 200);
  ASSERT_EQ(conn.getNotes(username, password).size(), 4);

  // remove + remove wrong idx + remove wrong idx + check notes cnt
  ASSERT_EQ(conn.removeNote(username, password, 3), 200);
  ASSERT_EQ(conn.removeNote(username, password, 3), 400);
  ASSERT_EQ(conn.removeNote(username, password, -1), 400);
  ASSERT_EQ(conn.getNotes(username, password).size(), 3);

  // remove user + check notes are empty
  ASSERT_EQ(conn.removeUser(username, password), 200);
  ASSERT_EQ(conn.getNotes(username, password).size(), 0);
}

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  testing::InitGoogleTest(&argc, argv);
  FLAGS_alsologtostderr = 1;
  QApplication app{argc, argv};

  return RUN_ALL_TESTS() && app.exec();
}