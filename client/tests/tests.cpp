#include <gtest/gtest.h>
#include <glog/logging.h>
#include <boost/asio.hpp>

#include "Service.hpp"
#include <QApplication>

auto sendRequest(QString data, QString contentType, QString stringUrl) {
    static QNetworkAccessManager manager;

    QNetworkRequest request;
    QUrl url = stringUrl;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    LOG(INFO) << "URL is " << url.toString();

    auto resp = manager.post(request, data.toUtf8());
    QEventLoop loop;
    QObject::connect(resp, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (resp->error() != 0) {
        LOG(ERROR) << resp->error();
    }
    LOG(INFO) << "Status: " << resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    return resp->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

TEST(Service, TestConnector) {
    QString baseUrl = "http://localhost:8192";
    int res;

    res = sendRequest("200", "plain/text", baseUrl + QString{"/save-status"});
    ASSERT_EQ(res, 200);

    Connector conn{baseUrl};

    res = conn.checkCreds("123", "123");
    ASSERT_EQ(res, 200);
}

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    testing::InitGoogleTest(&argc, argv);
    FLAGS_alsologtostderr = 1;
    QApplication app{argc, argv};

    return RUN_ALL_TESTS() && app.exec();
}