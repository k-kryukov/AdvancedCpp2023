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
#include <QJsonArray>
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

    auto getNotes(std::string const& username, std::string const& passwordHash) {
        QNetworkRequest request;
        QUrlQuery query;

        QUrl url = url_.toString() + QString{"/notes"};

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

        auto success = resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() / 100 == 2;
        if (!success) {
            LOG(ERROR) << "HTTP status code: " << resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            return std::vector<std::string>{};
        }

        QByteArray reply = resp->readAll();
        QJsonDocument document = QJsonDocument::fromJson(reply);
        QJsonObject json = document.object();

        LOG(INFO) << "Dumping notes...";
        auto notes = json[username.data()].toArray();
        auto rv = std::vector<std::string>{};
        rv.reserve(notes.size());

        std::for_each(notes.begin(), notes.end(),
            [&rv] (auto&& note) {
                rv.push_back(note.toString().toStdString());
            }
        );

        return rv;
    }

    bool createNewUser(std::string const& username, std::string const& password) {
        QNetworkRequest request;
        QUrlQuery query;

        QUrl url = url_.toString() + QString{"/users"};

        QJsonObject json;
        json["username"] = QString{username.data()};
        json["password"] = QString{password.data()};

        LOG(INFO) << "URL is " << url.toString().toStdString();
        auto jsonDoc = QJsonDocument{json}.toJson();

        request.setUrl(url);
        auto resp = manager.post(request, jsonDoc);
        QEventLoop loop;
        QObject::connect(resp, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (resp->error() != 0)
            LOG(ERROR) << "Error: " << resp->errorString().toStdString();

        LOG(INFO) << "Status: " << resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        return resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() / 100 == 2;
    }

    bool createNote(std::string const& username, std::string const& password, std::string const& noteText) {
        QNetworkRequest request;
        QUrlQuery query;

        QUrl url = url_.toString() + QString{"/notes"};

        QJsonObject json;
        json["username"] = QString{username.data()};
        json["password"] = QString{password.data()};
        json["noteText"] = QString{noteText.data()};

        LOG(INFO) << "URL is " << url.toString().toStdString();
        auto jsonDoc = QJsonDocument{json}.toJson();

        request.setUrl(url);
        auto resp = manager.post(request, jsonDoc);
        QEventLoop loop;
        QObject::connect(resp, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (resp->error() != 0)
            LOG(ERROR) << "Error: " << resp->errorString().toStdString();

        LOG(INFO) << "Status: " << resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        return resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() / 100 == 2;
    }

    bool removeNote(std::string const& username, std::string const& password, unsigned num) {
        QNetworkRequest request;
        QUrlQuery query;

        QUrl url = url_.toString() + QString{"/notes"};

        query.addQueryItem("username", username.data());
        query.addQueryItem("password", password.data());
        query.addQueryItem("note", std::to_string(num).data());

        url.setQuery(query.query());
        LOG(INFO) << url.toString().toStdString();
        request.setUrl(url);

        auto resp = manager.deleteResource(request);
        QEventLoop loop;
        QObject::connect(resp, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (resp->error() != 0)
            LOG(ERROR) << "Error: " << resp->errorString().toStdString();

        LOG(INFO) << "Status: " << resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        return resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() / 100 == 2;
    }
};