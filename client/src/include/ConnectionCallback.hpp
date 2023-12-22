#include <QObject>

class ConnectionCallback : public QObject {
public:
    void operator()() {
        // LOG(INF)
    }
};