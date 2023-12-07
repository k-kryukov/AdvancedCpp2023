#include <utility>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <glog/logging.h>
#include <nlohmann/json.hpp>

#include "Service.hpp"
#include "Handler.hpp"
#include "Session.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;

using namespace nlohmann;

class Server {
    unsigned port_;
    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor;
    Handler handler_;
    asio::ip::tcp::socket socket_;

public:
    explicit Server(unsigned port)
        : port_{port}, acceptor{io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)}, socket_{io_context} {}

    void start_ctx() { io_context.run(); }

    void dispatch(asio::ip::tcp::socket& socket) {
        LOG(INFO) << "Reading request!";
        beast::flat_buffer buffer;
        beast::http::request<beast::http::string_body> request;
        beast::http::read(socket, buffer, request);
        LOG(INFO) << "Request->target is " << request.target();

        auto&& tgt = request.target();
        if (tgt == "/users")
            beast::http::write(socket, dispatchUsers(socket, std::move(request)));
        else if (tgt == "/notes") {
            beast::http::write(socket, dispatchNotes(socket, std::move(request)));
        }
        else {
            beast::http::write(socket, handler_.handleUnexpectedRequest(socket, std::move(request)));
        }
    }

    Handler::ResponseType dispatchUsers(asio::ip::tcp::socket& socket, beast::http::request<beast::http::string_body> request) {
        if (request.method() == boost::beast::http::verb::post) {
            auto resp = handler_.handleRegisterRequest(socket, std::move(request));
            return resp;
        }
        else if (request.method() == boost::beast::http::verb::get) {
            auto resp = handler_.handleGetUsers(socket, std::move(request));
            return resp;
        }
        else if (request.method() == boost::beast::http::verb::delete_) {
            auto resp = handler_.handleRemoveUserRequest(socket, std::move(request));
            return resp;
        }
        else {
            auto resp = handler_.handleUnexpectedRequest(socket, std::move(request));
            return resp;
        }
    }

    Handler::ResponseType dispatchNotes(asio::ip::tcp::socket& socket, beast::http::request<beast::http::string_body> request) {
        if (request.method() == boost::beast::http::verb::post) {
            auto resp = handler_.handleNoteCreation(socket, std::move(request));
            return resp;
        }
        else if (request.method() == boost::beast::http::verb::get) {
            auto resp = handler_.handleGetNotes(socket, std::move(request));
            return resp;
        }
        else {
            auto resp = handler_.handleUnexpectedRequest(socket, std::move(request));
            return resp;
        }
    }

    void start() {
        socket_ = asio::ip::tcp::socket{io_context};

        // I would like to create Sessions and move socket there
        LOG(INFO) << "Accepting on " << port_;
        acceptor.async_accept(socket_, [this] (auto err) {
            LOG(INFO) << "Accepted!";

            try {
                dispatch(socket_);
            } catch (std::exception& ex) {
                LOG(ERROR) << "Caught unexpected error: " << ex.what();

                auto resp = ResponseWrapper{};
                beast::http::write(
                    socket_,
                    resp.result(beast::http::status::internal_server_error).get()
                );
            }

            LOG(INFO) << "Recursive call!";
            start();
        });
    }
};