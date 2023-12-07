#pragma once

#include <utility>
#include <iostream>
#include <thread>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/asio/thread_pool.hpp>

#include <glog/logging.h>
#include <nlohmann/json.hpp>

#include "Service.hpp"
#include "ResponseWrapper.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;

using namespace nlohmann;

class Handler {
    Service service_;
    boost::asio::thread_pool tp_{std::thread::hardware_concurrency()};

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

            if (service_.userExists(userName))
                return response.result(beast::http::status::bad_request);

            boost::asio::post(
                tp_, [&service = service_, userName = std::move(userName), passwordHashed] () {
                    service.createUser(userName, passwordHashed);
                }
            );
        }
        catch (json::exception const& err) {
            LOG(INFO) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
        }
        catch (std::runtime_error const& err) {
            LOG(INFO) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
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
            if (!service_.credsValid(userName, passwordHashed))
                return response.result(beast::http::status::bad_request);

            boost::asio::post(
                tp_, [&service = service_, userName = std::move(userName), passwordHashed] () {
                    service.removeUser(userName, passwordHashed);
                }
            );
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
        ResponseWrapper response;

        LOG(INFO) << "Generating response for register request!";

        json jsonBody = service_.getUsers();
        return response.version(request.version())
                       .result(beast::http::status::ok)
                       .set(beast::http::field::content_type, "text/json")
                       .body(jsonBody.dump())
                       .prepare_payload();
    }

    ResponseType handleUnexpectedRequest(asio::ip::tcp::socket& socket, beast::http::request<beast::http::string_body> request) {
        ResponseWrapper response;

        LOG(INFO) << "Generating response for register request!";
        return response.version(request.version())
                       .result(beast::http::status::not_found)
                       .prepare_payload();
    }

    ResponseType handleGetNotes(asio::ip::tcp::socket& socket, beast::http::request<beast::http::string_body> request) {
        ResponseWrapper response;

        LOG(INFO) << "Generating response for GetNotes request!";

        auto&& body = request.body();
        auto parsedBody = nlohmann::json::parse(body, nullptr, false);
        if (parsedBody.is_discarded()) {
            LOG(INFO) << "Generating response for GetNotes request [non JSON]!";

            return response.result(beast::http::status::unprocessable_entity);
        }

        json jsonBody;
        std::string userName;
        uint64_t passwordHashed;
        try {
            userName = parsedBody.at("username");
            passwordHashed = parsedBody.at("password");
            auto&& notes = service_.getNotesList(userName);
            jsonBody[userName] = std::vector<std::string>{};

            std::for_each(
                notes.begin(), notes.end(),
                [&jsonBody, &userName] (auto note) { jsonBody.at(userName).push_back(note->getText()); }
            );
        }
        catch (json::exception const& err) {
            LOG(INFO) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
        }
        catch (std::runtime_error const& err) {
            LOG(INFO) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
        }

        return response.version(request.version())
                       .result(beast::http::status::ok)
                       .set(beast::http::field::content_type, "text/json")
                       .body(jsonBody.dump())
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
            if (!service_.credsValid(userName, passwordHashed))
                return resp.result(beast::http::status::bad_request);

            boost::asio::post(
                tp_, [&service = service_, userName = std::move(userName), passwordHashed, noteText = std::move(noteText)] () {
                    service.addNote(userName, passwordHashed, std::make_shared<Note>(noteText));
                }
            );
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

    ~Handler() {
        tp_.join();
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