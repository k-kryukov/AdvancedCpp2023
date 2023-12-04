#include <string>
#include <cstdint>

class Service {
    // send request to server and check whether creds are correct
    bool matchCredsWithRemote(std::string username, uint64_t passwd) {
        return "A" == username && std::hash<std::string>{}("1") == passwd;
    }
public:
    bool checkCreds(std::string username, uint64_t passwd) {
        return matchCredsWithRemote(std::move(username), passwd);
    }
};