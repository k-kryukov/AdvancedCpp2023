#include <glog/logging.h>
#include <QTest>
#include <QApplication>
#include <QString>
#include <boost/asio.hpp>

#include "Service.hpp"

class ClientTests : public QObject
{
	Q_OBJECT

	Connector conn;
	int res;
	QString username{"krecov"};
	QString password{"123"};

private slots:
	void TestConnectorUsersManager()
	{
		// check + create + check
		QCOMPARE(conn.checkCreds("krecov", "123"), 404);
		QCOMPARE(conn.createNewUser("krecov", "123"), 200);
		QCOMPARE(conn.checkCreds("krecov", "123"), 200);

		// create existing user + existing with different password
		QCOMPARE(conn.createNewUser("krecov", "123"), 400);
		QCOMPARE(conn.createNewUser("krecov", "1234"), 400);

		// add + check + remove + check
		QCOMPARE(conn.createNewUser("krecov1", "123"), 200);
		QCOMPARE(conn.checkCreds("krecov1", "1234"), 404);
		QCOMPARE(conn.removeUser("krecov1", "123"), 200);
		QCOMPARE(conn.checkCreds("krecov1", "123"), 404);

		// remove with wrong password + check + remove + check
		QCOMPARE(conn.removeUser("krecov", "1234"), 400);
		QCOMPARE(conn.checkCreds("krecov", "123"), 200);
		QCOMPARE(conn.removeUser("krecov", "123"), 200);
		QCOMPARE(conn.checkCreds("krecov", "123"), 404);

		QCOMPARE(conn.getUsers().empty(), true);
	}

	void mySecondTest()
	{
		// create + check
		QCOMPARE(conn.createNewUser(username, password), 200);
		QCOMPARE(conn.checkCreds(username, password), 200);

		// create N times + check notes cnt
		QCOMPARE(conn.createNote(username, password, "Note1"), 200);
		QCOMPARE(conn.createNote(username, password, "Note2"), 200);
		QCOMPARE(conn.createNote(username, password, "Note1"), 200);
		QCOMPARE(conn.createNote(username, password, "AgainNote1"), 200);
		QCOMPARE(conn.getNotes(username, password).size(), 4);

		// remove + remove wrong idx + remove wrong idx + check notes cnt
		QCOMPARE(conn.removeNote(username, password, 3), 200);
		QCOMPARE(conn.removeNote(username, password, 3), 400);
		QCOMPARE(conn.removeNote(username, password, -1), 400);
		QCOMPARE(conn.getNotes(username, password).size(), 3);

		// remove user + check notes are empty
		QCOMPARE(conn.removeUser(username, password), 200);
		QCOMPARE(conn.getNotes(username, password).size(), 0);
	}
};

QTEST_MAIN(ClientTests)
#include "tests.moc"