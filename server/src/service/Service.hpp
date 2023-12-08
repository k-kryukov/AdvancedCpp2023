#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <thread>
#include <shared_mutex>
#include <mutex>

#include <boost/functional/hash.hpp>

#include "ServiceErrorCategory.hpp"
#include "UserData.hpp"

class Service {
    std::unordered_map<std::string, std::string> users; // username -> pwd hash
    std::unordered_map<std::string, std::shared_ptr<UserData>> userData;
    std::shared_mutex mtx_;
    // mutex is shared_mutex here just for future enhancements
    // currently std::mutex is rather suitable + std::shared_mutex might impact perfomance since it usually makes system call

public:
    Service() = default;
    Service(Service const&) = delete;
    Service(Service &&) = default;

    void createUser(std::string const& userName, std::string pswdHash) {
        std::unique_lock lock{mtx_};
        checkUserNonExistance(userName);

        users.emplace(userName, pswdHash);
        userData.emplace(userName, std::make_shared<UserData>());
    }

    auto getUsers() {
        std::shared_lock lock{mtx_};
        std::vector<std::string> usersList{users.size()};
        std::transform(users.begin(), users.end(), usersList.begin(), [] (auto kv) { return kv.first; });

        return usersList;
    }

    void removeUser(std::string const& userName, std::string pswdHash) {
        std::unique_lock lock{mtx_};
        checkUserCreds(userName, pswdHash);

        users.erase(userName);
        userData.erase(userName);
    }

    uint64_t addNote(std::string const& userName, std::string pswdHash, std::shared_ptr<Note> note) {
        std::unique_lock lock{mtx_};
        checkUserExistance(userName);

        return userData[userName]->addNote(note);
    }

    void removeNote(std::string const& userName, uint64_t id) {
        std::unique_lock lock{mtx_};
        checkUserExistance(userName);

        userData[userName]->removeNote(id);
    }

    auto getNotesList(std::string const& userName) {
        std::shared_lock lock{mtx_};
        checkUserExistance(userName);

        return userData[userName]->getNotes();
    }

    bool userExists(std::string const& userName) noexcept {
        std::shared_lock lock{mtx_};
        if (!users.contains(userName)) {
            LOG(WARNING) << "User " << userName << " does not exist!";
            return false;
        }

        return true;
    }

    bool credsValid(std::string const& userName, std::string const& pswdHash) {
        std::shared_lock lock{mtx_};
        auto res = userExists(userName) && pswdHash == users[userName];

        if (userExists(userName))
            LOG(INFO) << "Real password is " << users[userName];

        return res;
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

    void checkUserCreds(std::string const& userName, std::string pswdHash) {
        checkUserExistance(userName);

        if (pswdHash != users[userName]) {
            std::stringstream ss;
            ss << "Wrong password";

            throw std::runtime_error{std::move(ss).str()};
        }
    }
};