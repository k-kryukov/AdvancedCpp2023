#pragma once

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

    bool createUser(std::string username, std::string const& passwd) {
        return conn.createNewUser(std::move(username), passwd);
    }

    auto getNotes(std::string username, std::string const& passwd) {
        return conn.getNotes(std::move(username), passwd);
    }

    auto createNote(std::string username, std::string const& passwd, std::string const& noteText) {
        return conn.createNote(std::move(username), passwd, noteText);
    }

    auto removeNote(std::string username, std::string const& passwd, unsigned noteNumber) {
        return conn.removeNote(std::move(username), passwd, noteNumber);
    }
};