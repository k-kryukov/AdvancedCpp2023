#pragma once

#include <string>

#include <QObject>
#include <QUrl>
#include <QUrlQuery>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QEventLoop>

class Connector {
    QUrl url_{"http://localhost:12345"};
    QNetworkAccessManager manager;

public:
    Connector(std::string const& s) : url_(QString{s.data()}) {}
    Connector() {}

    bool checkCreds(std::string const& username, std::string const& passwordHash) {
        QNetworkRequest request;
        QUrlQuery query;

        QUrl url = url_.toString() + QString{"/validate-creds"};

        query.addQueryItem("username", username.data());
        query.addQueryItem("password", passwordHash.data());

        url.setQuery(query.query());
        LOG(INFO) << url.toString().toStdString();

        request.setUrl(url);

        auto resp = manager.get(request);
        QEventLoop loop;
        QObject::connect(resp, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (resp->error() != 0) {
            LOG(ERROR) << resp->error();
        }
        LOG(INFO) << "Status: " << resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        return resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() / 100 == 2;
    }

    bool createNewUser(std::string const& username, std::string const& password) {
        QNetworkRequest request;
        QUrlQuery query;

        QUrl url = url_.toString() + QString{"/users"};

        QJsonObject json;
        json["username"] = QString{username.data()};
        json["password"] = QString{password.data()};

        auto resp = manager.post(request, QJsonDocument{json}.toJson());
        QEventLoop loop;
        QObject::connect(resp, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (resp->error() != 0) {
            LOG(ERROR) << resp->error();
        }
        LOG(INFO) << "Status: " << resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        return resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() / 100 == 2;
    }
};