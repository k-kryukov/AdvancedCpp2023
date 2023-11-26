#include <utility>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <glog/logging.h>
#include <nlohmann/json.hpp>

#include "Service.hpp"
#include "Handler.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;

using namespace nlohmann;

class Server {
    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor;
    Handler handler_;
public:
    explicit Server(unsigned port) : acceptor{io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)} {}

    void dispatch(asio::ip::tcp::socket& socket) {
        LOG(INFO) << "Reading request!";
        beast::flat_buffer buffer;
        beast::http::request<beast::http::string_body> request;
        beast::http::read(socket, buffer, request);
        LOG(INFO) << "Request->target is " << request.target();

        auto&& tgt = request.target();
        if (tgt == "/users")
            beast::http::write(socket, dispatchUsers(socket, std::move(request)));
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

    void start() {
        while (true) {
            asio::ip::tcp::socket socket(io_context);

            LOG(INFO) << "Accepting...";
            acceptor.accept(socket);
            LOG(INFO) << "Accepted!";

            dispatch(socket);

            socket.shutdown(asio::ip::tcp::socket::shutdown_send);
        }
    }
};