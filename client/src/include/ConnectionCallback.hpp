#include <sstream>

#include <QObject>
#include <QNetworkReply>

class ConnectionCallback {
public:
    static auto handleFinished(QNetworkReply* reply, std::string op) {
        QMessageBox messageBox;
        messageBox.setFixedSize(500,200);
        auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        std::stringstream ss;

        if (status / 100 != 2) {
            ss << op << ": error, status is " << status;
            messageBox.information(nullptr, std::move(ss).str().data(), "");
        }
        else {
            ss << op << ": done";
            messageBox.information(nullptr, std::move(ss).str().data(), "");
        }
    }
};