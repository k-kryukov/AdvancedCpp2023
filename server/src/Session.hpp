#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <glog/logging.h>
#include <nlohmann/json.hpp>

#include "Handler.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;

class Session {
    asio::ip::tcp::socket socket_;
    Handler& handler_;

public:
    Session(asio::ip::tcp::socket socket, Handler& handler)
        : socket_{std::move(socket)}, handler_{handler} {}

    void handle() {
        try {
            dispatch();
        } catch (std::exception& ex) {
            LOG(ERROR) << "Caught unexpected error: " << ex.what();

            auto resp = ResponseWrapper{};
            beast::http::write(
                socket_,
                resp.result(beast::http::status::internal_server_error).get()
            );
        }

    }

    void dispatch() {
        LOG(INFO) << "Reading request!";
        beast::flat_buffer buffer;
        beast::http::request<beast::http::string_body> request;
        beast::http::read(socket_, buffer, request);
        LOG(INFO) << "Request->target is " << request.target();

        auto&& tgt = request.target();
        if (tgt == "/users")
            beast::http::write(socket_, dispatchUsers(std::move(request)));
        else if (tgt == "/notes") {
            beast::http::write(socket_, dispatchNotes(std::move(request)));
        }
        else {
            beast::http::write(socket_, handler_.handleUnexpectedRequest(socket_, std::move(request)));
        }
    }

    Handler::ResponseType dispatchUsers(beast::http::request<beast::http::string_body> request) {
        if (request.method() == boost::beast::http::verb::post) {
            auto resp = handler_.handleRegisterRequest(socket_, std::move(request));
            return resp;
        }
        else if (request.method() == boost::beast::http::verb::get) {
            auto resp = handler_.handleGetUsers(socket_, std::move(request));
            return resp;
        }
        else if (request.method() == boost::beast::http::verb::delete_) {
            auto resp = handler_.handleRemoveUserRequest(socket_, std::move(request));
            return resp;
        }
        else {
            auto resp = handler_.handleUnexpectedRequest(socket_, std::move(request));
            return resp;
        }
    }

    Handler::ResponseType dispatchNotes(beast::http::request<beast::http::string_body> request) {
        if (request.method() == boost::beast::http::verb::post) {
            auto resp = handler_.handleNoteCreation(socket_, std::move(request));
            return resp;
        }
        else if (request.method() == boost::beast::http::verb::get) {
            auto resp = handler_.handleGetNotes(socket_, std::move(request));
            return resp;
        }
        else {
            auto resp = handler_.handleUnexpectedRequest(socket_, std::move(request));
            return resp;
        }
    }
};