#include <string>
#include <cstdint>

#include "Connector.hpp"

class Service {
    Connector conn{};

    // send request to server and check whether creds are correct
    bool matchCredsWithRemote(std::string username, std::string passwd) {
        // return "A" == username && std::hash<std::string>{}("1") == passwd;
        return conn.checkCreds(username, passwd);
    }
public:
    bool checkCreds(std::string username, std::string const& passwd) {
        return matchCredsWithRemote(std::move(username), passwd);
    }
};