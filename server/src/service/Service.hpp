#include <string>
#include <unordered_map>

#include <boost/functional/hash.hpp>

#include "ServiceErrorCategory.hpp"

template<typename Hash = std::hash<std::string>>
class Service {
    std::unordered_map<std::string, size_t> users; // username -> pwd hash
    Hash hash_{};
public:
    Service() = default;
    Service(Service const&) = delete;
    Service(Service &&) = default;

    std::error_code createUser(std::string const& userName, std::string const& pwdHash) {
        if (users.contains(userName)) {
            return std::error_code{
                ServiceError::USR_EXIST, ServiceErrorCategory{}
            };
        }
    }
};