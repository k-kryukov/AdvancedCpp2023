#pragma once

#include <string>
#include <cstdint>

#include "Connector.hpp"

class Service {
    Connector conn{};

    // send request to server and check whether creds are correct
    auto matchCredsWithRemote(QString username, QString passwd) {
        return conn.checkCreds(username, passwd);
    }

public:
    auto checkCreds(QString username, QString passwd) {
        return matchCredsWithRemote(std::move(username), passwd);
    }

    auto createUser(QString username, QString passwd) {
        return conn.createNewUser(std::move(username), passwd);
    }

    auto getNotes(QString username, QString passwd) {
        return conn.getNotes(std::move(username), passwd);
    }

    auto createNote(QString username, QString passwd, QString noteText) {
        return conn.createNote(std::move(username), passwd, noteText);
    }

    auto removeNote(QString username, QString passwd, unsigned noteNumber) {
        return conn.removeNote(std::move(username), passwd, noteNumber);
    }
};