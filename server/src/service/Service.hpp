#include <string>
#include <sstream>
#include <unordered_map>
#include <memory>

#include <boost/functional/hash.hpp>

#include "ServiceErrorCategory.hpp"
#include "UserData.hpp"

template<typename Hash = std::hash<std::string>>
class Service {
    std::unordered_map<std::string, size_t> users; // username -> pwd hash
    Hash hash_{};
    std::unordered_map<std::string, std::shared_ptr<UserData>> userData;

public:
    Service() = default;
    Service(Service const&) = delete;
    Service(Service &&) = default;

    void createUser(std::string const& userName, size_t pswdHash) {
        checkUserNonExistance(userName);

        users.emplace(userName, pswdHash);
        userData.emplace(userName, std::make_shared<UserData>());
    }

    void removeUser(std::string const& userName, size_t pswdHash) {
        checkUserCreds(userName, pswdHash);

        users.erase(userName);
        userData.erase(userName);
    }

    uint64_t addNote(std::string const& userName, std::shared_ptr<Note> note) {
        checkUserExistance(userName);

        return userData[userName]->addNote(note);
    }

    void removeNote(std::string const& userName, uint64_t id) {
        checkUserExistance(userName);

        userData[userName]->removeNote(id);
    }

    auto getNotesList(std::string const& userName) {
        checkUserExistance(userName);

        return userData[userName]->getNotes();
    }
private:
    void checkUserExistance(std::string const& userName) {
        if (!users.contains(userName)) {
            std::stringstream ss;
            ss << "User does not exist: " << userName;

            throw std::runtime_error{std::move(ss).str()};
        }
    }

    void checkUserNonExistance(std::string const& userName) {
        if (users.contains(userName)) {
            std::stringstream ss;
            ss << "User already exists: " << userName;

            throw std::runtime_error{std::move(ss).str()};
        }
    }

    void checkUserCreds(std::string const& userName, size_t pswdHash) {
        checkUserExistance(userName);

        if (pswdHash != users[userName]) {
            std::stringstream ss;
            ss << "Wrong password";

            throw std::runtime_error{std::move(ss).str()};
        }
    }
};