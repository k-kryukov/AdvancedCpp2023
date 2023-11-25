#include <utility>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <glog/logging.h>
#include <nlohmann/json.hpp>

#include "Service.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;

using namespace nlohmann;

class Server {
    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor;
    Service service_;
public:
    explicit Server(unsigned port) : acceptor{io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)} {}

    void dispatch(asio::ip::tcp::socket& socket) {
        LOG(INFO) << "Reading request!";
        beast::flat_buffer buffer;
        beast::http::request<beast::http::string_body> request;
        beast::http::read(socket, buffer, request);
        LOG(INFO) << "Request->target is " << request.target();

        auto&& tgt = request.target();
        if (tgt == "/register") {
            auto resp = handleRegisterRequest(socket, std::move(request));
            beast::http::write(socket, resp);
        }
        else if (tgt == "/users") {
            auto resp = handleGetUsers(socket, std::move(request));
            beast::http::write(socket, resp);
        }

        // auto fields = request.at("Content-Type");
        // LOG(INFO) << "Fields are " << fields;
        // auto body = request.body();
        // auto x = nlohmann::json::parse(body);

        // LOG(INFO) << "Body is " << x;
        // LOG(INFO) << "Header is " << request.keep_alive();
    }

    void start() {
        // start listening
        while (true) {
            asio::ip::tcp::socket socket(io_context);

            LOG(INFO) << "Accepting...";
            acceptor.accept(socket);
            LOG(INFO) << "Accepted!";

            dispatch(socket);

            // auto fields = request.at("Content-Type");
            // LOG(INFO) << "Fields are " << fields;
            // auto body = request.body();
            // auto x = nlohmann::json::parse(body);

            // LOG(INFO) << "Body is " << x;
            // LOG(INFO) << "Header is " << request.keep_alive();

            socket.shutdown(asio::ip::tcp::socket::shutdown_send);
        }
    }
private:
    beast::http::response<beast::http::string_body> handleRegisterRequest(asio::ip::tcp::socket& socket, beast::http::request<beast::http::string_body> request) {
        beast::http::response<beast::http::string_body> response;

        auto&& body = request.body();
        auto parsedBody = nlohmann::json::parse(body, nullptr, false);
        if (parsedBody.is_discarded()) {
            LOG(INFO) << "Generating response for register request [non JSON]!";

            response.result(beast::http::status::unprocessable_entity);
            return response;
        }

        auto userName = parsedBody.at("username");
        auto passwordHashed = parsedBody.at("password");

        try {
            service_.createUser(userName, passwordHashed);
        }
        catch (std::runtime_error const& err) {
            LOG(INFO) << "Generating response for register request [non JSON]!";

            response.result(beast::http::status::bad_request);
            return response;
        }

        LOG(INFO) << "Users are:";
        for (auto user : service_.getUsers()) {

        }

        LOG(INFO) << "Generating response for register request!";
        response.version(request.version());
        response.result(beast::http::status::ok);
        response.set(beast::http::field::content_type, "text/plain");
        response.prepare_payload();

        // Отправляем ответ
        return response;
    }

    beast::http::response<beast::http::string_body> handleGetUsers(asio::ip::tcp::socket& socket, beast::http::request<beast::http::string_body> request) {
        beast::http::response<beast::http::string_body> response;

        LOG(INFO) << "Users are:";

        LOG(INFO) << "Generating response for register request!";
        response.version(request.version());
        response.result(beast::http::status::ok);
        response.set(beast::http::field::content_type, "text/plain");
        response.prepare_payload();

        std::string users = boost::algorithm::join(service_.getUsers(), " ");
        response.body() = users;

        // Отправляем ответ
        return response;
    }
};