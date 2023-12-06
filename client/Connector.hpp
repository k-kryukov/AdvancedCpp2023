#include <string>

#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>

class Connector {
    QUrl url_{"http://localhost:12345"};
    QNetworkAccessManager manager;

public:
    Connector(std::string const& s) : url_(QString{s.data()}) {}

    bool checkCreds(std::string const& username, uint64_t passwordHash) {
        QNetworkRequest request;
        request.setUrl(url_.toString() + QString{"/users"});

        QJsonObject obj;
        obj["username"] = QString{username.data()};
        obj["password"] = QString::number(passwordHash);

        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();
        auto resp = manager.post(request, data);

        return !resp->error();
    }
};