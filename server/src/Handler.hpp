#include <utility>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <glog/logging.h>
#include <nlohmann/json.hpp>

#include "Service.hpp"
#include "ResponseWrapper.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;

using namespace nlohmann;

class Handler {
    Service service_;
public:
    using ResponseType = beast::http::response<beast::http::string_body>;

    ResponseType handleRegisterRequest(asio::ip::tcp::socket& socket, beast::http::request<beast::http::string_body> request) {
        ResponseWrapper response;

        auto&& body = request.body();
        auto parsedBody = nlohmann::json::parse(body, nullptr, false);
        if (parsedBody.is_discarded()) {
            LOG(INFO) << "Generating response for register request [non JSON]!";

            return response.result(beast::http::status::unprocessable_entity);
        }

        try {
            std::string userName = parsedBody.at("username");
            uint64_t passwordHashed = parsedBody.at("password");
            service_.createUser(userName, passwordHashed);
        }
        catch (json::exception const& err) {
            LOG(INFO) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
        }
        catch (std::runtime_error const& err) {
            LOG(INFO) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
        }

        LOG(INFO) << "Users are:";
        for (auto user : service_.getUsers()) {
            LOG(INFO) << user;
        }

        LOG(INFO) << "Generating response for register request!";
        return response.version(request.version())
                       .result(beast::http::status::ok)
                       .set(beast::http::field::content_type, "text/plain")
                       .prepare_payload();
    }

    ResponseType handleRemoveUserRequest(asio::ip::tcp::socket& socket, beast::http::request<beast::http::string_body> request) {
        ResponseWrapper response;

        auto&& body = request.body();
        auto parsedBody = nlohmann::json::parse(body, nullptr, false);
        if (parsedBody.is_discarded()) {
            LOG(INFO) << "Generating response for register request [non JSON]!";

            return response.result(beast::http::status::unprocessable_entity);
        }

        try {
            std::string userName = parsedBody.at("username");
            uint64_t passwordHashed = parsedBody.at("password");
            service_.removeUser(userName, passwordHashed);
        }
        catch (json::exception const& err) {
            LOG(INFO) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
        }
        catch (std::runtime_error const& err) {
            LOG(INFO) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
        }

        LOG(INFO) << "Users are:";
        for (auto user : service_.getUsers()) {
            LOG(INFO) << user;
        }

        LOG(INFO) << "Generating response for register request!";
        return response.version(request.version())
                       .result(beast::http::status::ok)
                       .set(beast::http::field::content_type, "text/plain")
                       .prepare_payload();
    }

    ResponseType handleGetUsers(asio::ip::tcp::socket& socket, beast::http::request<beast::http::string_body> request) {
        ResponseType response;

        LOG(INFO) << "Generating response for register request!";
        response.version(request.version());
        response.result(beast::http::status::ok);
        response.set(beast::http::field::content_type, "text/plain");

        LOG(INFO) << "Users are:";
        std::string users = boost::algorithm::join(service_.getUsers(), " ");
        response.body() = users;
        LOG(INFO) << users;

        response.prepare_payload();

        // Отправляем ответ
        return response;
    }

    ResponseType handleUnexpectedRequest(asio::ip::tcp::socket& socket, beast::http::request<beast::http::string_body> request) {
        ResponseWrapper response;

        LOG(INFO) << "Generating response for register request!";
        return response.version(request.version())
                       .result(beast::http::status::not_found)
                       .prepare_payload();
    }

    ResponseType handleNoteCreation(asio::ip::tcp::socket& socket, beast::http::request<beast::http::string_body> request) {
        auto&& body = request.body();
        auto parsedBody = nlohmann::json::parse(body, nullptr, false);
        ResponseWrapper resp;

        if (parsedBody.is_discarded()) {
            LOG(INFO) << "Generating response for register request [non JSON]!";

            return resp.result(beast::http::status::unprocessable_entity);
        }

        try {
            std::string userName = parsedBody.at("username");
            uint64_t passwordHashed = parsedBody.at("password");
            std::string noteText = parsedBody.at("noteText");
            service_.addNote(userName, passwordHashed, std::make_shared<Note>(noteText));
        }
        catch (json::exception const& err) {
            LOG(INFO) << "Error: " << err.what();

            return resp.result(beast::http::status::bad_request);
        }
        catch (std::runtime_error const& err) {
            LOG(INFO) << "Error: " << err.what();

            return resp.result(beast::http::status::bad_request);
        }

        LOG(INFO) << "Generating response for register request!";
        return resp.version(request.version())
                   .result(beast::http::status::ok)
                   .set(beast::http::field::content_type, "text/plain")
                   .prepare_payload();
    }
private:
    ResponseType prepareResponse(beast::http::status status, unsigned version) {
        ResponseWrapper response;

        return response.version(version)
                       .result(status)
                       .set(beast::http::field::content_type, "text/plain")
                       .prepare_payload();
    }
};