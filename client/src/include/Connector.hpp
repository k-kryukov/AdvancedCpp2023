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
#include <QMessageBox>

#include "QTExtended.hpp"
#include "ConnectionCallback.hpp"

class Connector {
    QUrl url_{"http://localhost:12345"};
    QNetworkAccessManager manager;
    QEventLoop loop;

public:
    Connector(QString s) : url_(s) { }
    Connector() {}

    Connector(Connector const&) = delete;
    Connector(Connector&&) = default;

    auto& operator=(Connector const&) = delete;
    Connector& operator=(Connector&&) = default;

    ~Connector() {
        loop.exit();
    }

    auto checkCreds(QString username, QString passwordHash) {
        QNetworkRequest request;
        QUrlQuery query;

        QUrl url = url_.toString() + QString{"/validate-creds"};

        query.addQueryItem("username", username);
        query.addQueryItem("password", passwordHash);

        url.setQuery(query.query());
        LOG(INFO) << url.toString();

        request.setUrl(url);

        auto resp = manager.get(request);
        QObject::connect(resp, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        if (!loop.isRunning())
            loop.exec();

        if (resp->error() != 0) {
            LOG(ERROR) << resp->errorString();
        }
        LOG(INFO) << "Status: " << resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        return resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    }

    auto getNotes(QString username, QString passwordHash) {
        QNetworkRequest request;
        QUrlQuery query;

        QUrl url = url_.resolved(QString{"/notes"});

        query.addQueryItem("username", username);
        query.addQueryItem("password", passwordHash);

        url.setQuery(query.query());
        LOG(INFO) << url.toString();

        request.setUrl(url);

        auto resp = manager.get(request);
        QObject::connect(resp, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        if (!loop.isRunning())
            loop.exec();

        if (resp->error() != 0) {
            LOG(ERROR) << resp->error();
        }
        LOG(INFO) << "Status: " << resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        auto success = resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() / 100 == 2;
        if (!success) {
            LOG(ERROR) << "HTTP status code: " << resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            return std::vector<QString>{};
        }

        QByteArray reply = resp->readAll();
        QJsonDocument document = QJsonDocument::fromJson(reply);
        QJsonObject json = document.object();

        LOG(INFO) << "Dumping notes...";
        auto notes = json[username.data()].toArray();
        auto rv = std::vector<QString>{};
        rv.reserve(notes.size());

        std::for_each(notes.begin(), notes.end(),
            [&rv] (auto&& note) {
                rv.push_back(note.toString());
            }
        );

        return rv;
    }

    auto getUsers() {
        QNetworkRequest request;

        QUrl url = url_.resolved(QString{"/users"});
        request.setUrl(url);
        LOG(INFO) << url.toString();

        auto resp = manager.get(request);
        QObject::connect(resp, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        if (!loop.isRunning())
            loop.exec();

        if (resp->error() != 0) {
            LOG(ERROR) << resp->error();
        }
        LOG(INFO) << "Status: " << resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        auto success = resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() / 100 == 2;
        if (!success) {
            LOG(ERROR) << "HTTP status code: " << resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            return std::vector<QString>{};
        }

        QByteArray reply = resp->readAll();
        QJsonDocument document = QJsonDocument::fromJson(reply);
        QJsonObject json = document.object();

        LOG(INFO) << "Dumping users to array...";
        auto arr = document.array();
        auto rv = std::vector<QString>{};
        rv.reserve(arr.size());

        std::for_each(arr.begin(), arr.end(),
            [&rv] (auto&& user) {
                rv.push_back(user.toString());
            }
        );

        return rv;
    }

    int createNewUser(QString username, QString password) {
        QNetworkRequest request;
        QUrlQuery query;

        QUrl url = url_.toString() + QString{"/users"};

        QJsonObject json;
        json["username"] = QString{username.data()};
        json["password"] = QString{password.data()};

        LOG(INFO) << "URL is " << url.toString();
        auto jsonDoc = QJsonDocument{json}.toJson();

        request.setUrl(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        auto resp = manager.post(request, jsonDoc);
        QObject::connect(resp, &QNetworkReply::finished,
            [resp] () { ConnectionCallback::handleFinished(resp, "User creation"); }
        );
        if (!loop.isRunning())
            loop.exec();

        return 200;
    }

    auto createNote(QString username, QString password, QString noteText) {
        QNetworkRequest request;
        QUrlQuery query;

        QUrl url = url_.toString() + QString{"/notes"};

        QJsonObject json;
        json["username"] = QString{username.data()};
        json["password"] = QString{password.data()};
        json["noteText"] = QString{noteText.data()};

        LOG(INFO) << "URL is " << url.toString();
        auto jsonDoc = QJsonDocument{json}.toJson();

        request.setUrl(url);
        auto resp = manager.post(request, jsonDoc);
        QObject::connect(resp, &QNetworkReply::finished,
            [resp] () { ConnectionCallback::handleFinished(resp, "Note creation"); }
        );
        if (!loop.isRunning())
            loop.exec();

        return 200;
    }

    auto removeNote(QString username, QString password, unsigned num) {
        QNetworkRequest request;
        QUrlQuery query;

        QUrl url = url_.toString() + QString{"/notes"};

        query.addQueryItem("username", username);
        query.addQueryItem("password", password);
        query.addQueryItem("note", std::to_string(num).data());

        url.setQuery(query.query());
        LOG(INFO) << url.toString();
        request.setUrl(url);

        auto resp = manager.deleteResource(request);
        QObject::connect(resp, &QNetworkReply::finished,
            [resp] () { ConnectionCallback::handleFinished(resp, "Note removing"); }
        );
        if (!loop.isRunning())
            loop.exec();

        return 200;
    }

    int removeUser(QString username, QString password) {
        QNetworkRequest request;
        QUrlQuery query;

        QUrl url = url_.toString() + QString{"/users"};

        query.addQueryItem("username", username);
        query.addQueryItem("password", password);

        url.setQuery(query.query());
        LOG(INFO) << url.toString();
        request.setUrl(url);

        auto resp = manager.deleteResource(request);
        QObject::connect(resp, &QNetworkReply::finished,
            [resp] () { ConnectionCallback::handleFinished(resp, "User removing"); }
        );
        if (!loop.isRunning())
            loop.exec();

        return 200;
    }
};