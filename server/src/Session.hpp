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
    asio::io_context& context_;
    bool contextStarted_{false};
    constexpr static asio::chrono::seconds timeout{60}; // must be 60
    asio::steady_timer timer;

    Handler& handler_;

    using RequestType = beast::http::request<beast::http::string_body>;

public:
    Session(asio::ip::tcp::socket socket, Handler& handler, asio::io_context& context)
        : socket_{std::move(socket)}, handler_{handler}, context_{context}, timer{context_, timeout} {}

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
        timer.expires_after(timeout);
        timer.async_wait([sess = shared_from_this()] (auto err) {
            if (err)
                LOG(ERROR) << "Error while waiting timer: " << err;

            LOG(INFO) << "Closing socket...";
            sess->socket_.close();
        });

        LOG(INFO) << "Reading request!";
        auto buffer = std::make_shared<beast::flat_buffer>();
        auto request = std::make_shared<beast::http::request<beast::http::string_body>>();

        beast::http::async_read(socket_, *buffer, *request,
            [self = shared_from_this(), buffer, request] (auto err, auto cnt) {
                constexpr auto eof = 1;
                constexpr auto cancelled = 125;
                if (err.value() == eof) {
                    LOG(INFO) << "Socket is closed!"; return;
                }
                else if (err.value() == cancelled) {
                    LOG(ERROR) << "READ is cancelled!: "; return;
                }
                else if (err.value() != 0) {
                    LOG(ERROR) << "Unexpected error: " << err; return;
                }

                LOG(INFO) << "Request->target is " << request->target();

                auto&& tgt = request->target();

                auto resp = std::make_shared<Handler::ResponseType>(
                    (std::regex_search(tgt.data(), std::regex{"/users\?.+"})) ?
                        self->dispatchUsers(request)
                    : (std::regex_search(tgt.data(), std::regex{"/notes\?.+"})) ?
                        self->dispatchNotes(request)
                    : (std::regex_search(tgt.data(), std::regex{"/validate-creds\?.+"})) ?
                        self->dispatchValidateCreds(request)
                    :
                        self->handler_.handleUnexpectedRequest(self->socket_, request)
                );

                beast::http::async_write(self->socket_, *resp,
                    [resp, self] (auto err, auto bytes_transferred) {
                        constexpr auto cancelled = 125;
                        if (err.value() == cancelled) {
                            LOG(ERROR) << "READ is cancelled!: "; return;
                        }
                        else if (err.value()) {
                            LOG(ERROR) << "Unexpected error: " << err; return;
                        }

                        self->dispatch();
                    }
                );
            }
        );
    }

    Handler::ResponseType dispatchUsers(std::shared_ptr<RequestType> request) {
        DLOG(INFO) << "Req target is " << request->target() << ", method is " << request->method();
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