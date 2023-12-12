#pragma once

#include <regex>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <glog/logging.h>
#include <nlohmann/json.hpp>

#include "Handler.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;

class Session : public std::enable_shared_from_this<Session> {
    asio::ip::tcp::socket socket_;
    Handler& handler_;

    using RequestType = beast::http::request<beast::http::string_body>;

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
        auto buffer = std::make_shared<beast::flat_buffer>();
        auto request = std::make_shared<beast::http::request<beast::http::string_body>>();

        beast::http::async_read(socket_, *buffer, *request,
            [self = shared_from_this(), buffer, request] (auto err, auto cnt) {
                constexpr auto eof = 1;
                if (err.value() == eof) {
                    LOG(INFO) << "Socket is closed!"; return;
                }

                LOG(INFO) << "Request->target is " << request->target();

                auto&& tgt = request->target();

                auto resp = std::make_shared<Handler::ResponseType>(
                    (tgt == "/users") ?
                        self->dispatchUsers(request)
                    : (std::regex_search(tgt.data(), std::regex{"/notes\?.+"})) ?
                        self->dispatchNotes(request)
                    : (std::regex_search(tgt.data(), std::regex{"/validate-creds\?.+"})) ?
                        self->dispatchValidateCreds(request)
                    :
                        self->handler_.handleUnexpectedRequest(self->socket_, request)
                );

                beast::http::async_write(self->socket_, *resp,
                    [resp, self] (auto err, auto bytes_transferred) { /* self->dispatch(); */ }
                );
            }
        );
    }

    Handler::ResponseType dispatchUsers(std::shared_ptr<RequestType> request) {
        if (request->method() == boost::beast::http::verb::post) {
            auto resp = handler_.handleRegisterRequest(socket_, std::move(request));
            return resp;
        }
        else if (request->method() == boost::beast::http::verb::get) {
            auto resp = handler_.handleGetUsers(socket_, std::move(request));
            return resp;
        }
        else if (request->method() == boost::beast::http::verb::delete_) {
            auto resp = handler_.handleRemoveUserRequest(socket_, std::move(request));
            return resp;
        }
        else {
            auto resp = handler_.handleUnexpectedRequest(socket_, std::move(request));
            return resp;
        }
    }

    Handler::ResponseType dispatchNotes(std::shared_ptr<RequestType> request) {
        if (request->method() == boost::beast::http::verb::post) {
            auto resp = handler_.handleNoteCreation(socket_, std::move(request));
            return resp;
        }
        else if (request->method() == boost::beast::http::verb::get) {
            auto resp = handler_.handleGetNotes(socket_, std::move(request));
            return resp;
        }
        else if (request->method() == boost::beast::http::verb::delete_) {
            auto resp = handler_.handleDeleteNote(socket_, std::move(request));
            return resp;
        }
        else {
            auto resp = handler_.handleUnexpectedRequest(socket_, std::move(request));
            return resp;
        }
    }

    Handler::ResponseType dispatchValidateCreds(std::shared_ptr<RequestType> request) {
        LOG(INFO) << "Dispatching validate creds";
        if (request->method() == boost::beast::http::verb::get) {
            auto resp = handler_.handleValidateCreds(socket_, std::move(request));
            return resp;
        }
        else {
            auto resp = handler_.handleUnexpectedRequest(socket_, std::move(request));
            return resp;
        }
    }
};