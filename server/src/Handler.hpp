#pragma once

#include <utility>
#include <iostream>
#include <thread>
#include <unordered_map>

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


std::unordered_map<std::string, std::string> parseQueryParams(const std::string& url) {
    std::unordered_map<std::string, std::string> params;

    size_t position = url.find("?");
    if (position != std::string::npos) {
        std::string query = url.substr(position + 1);
        std::string key, value;
        LOG(INFO) << "Query is " << query;

        size_t pos = 0;
        while ((pos = query.find("&")) != std::string::npos) {
            std::string pair = query.substr(0, pos);
            size_t eqPos = pair.find("=");
            if (eqPos != std::string::npos) {
                key = pair.substr(0, eqPos);
                value = pair.substr(eqPos + 1);
                LOG(INFO) << "Inserted value is " << value;
                params[key] = value;
            }
            query.erase(0, pos + 1);
        }

        size_t eqPos = query.find("=");
        if (eqPos != std::string::npos) {
            key = query.substr(0, eqPos);
            value = query.substr(eqPos + 1);
            LOG(INFO) << "Inserted value is " << value;
            params[key] = value;
        }
    }

    return params;
}

class Handler {
    Service service_;
    boost::asio::thread_pool tp_{std::thread::hardware_concurrency()};

public:
    using ResponseType = beast::http::response<beast::http::string_body>;
    using RequestType = beast::http::request<beast::http::string_body>;

    ResponseType handleRegisterRequest(asio::ip::tcp::socket& socket, std::shared_ptr<RequestType> request) {
        ResponseWrapper response;

        auto&& body = request->body();
        auto parsedBody = nlohmann::json::parse(body, nullptr, false);
        if (parsedBody.is_discarded()) {
            LOG(INFO) << "Generating response for register request [non JSON]!";

            return response.result(beast::http::status::unprocessable_entity);
        }

        try {
            std::string userName = parsedBody.at("username");
            std::string passwordHashed = parsedBody.at("password");

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
        return response.version(request->version())
                       .result(beast::http::status::ok)
                       .set(beast::http::field::content_type, "text/plain")
                       .prepare_payload();
    }

    ResponseType handleRemoveUserRequest(asio::ip::tcp::socket& socket, std::shared_ptr<RequestType> request) {
        ResponseWrapper response;

        auto&& body = request->body();
        auto parsedBody = nlohmann::json::parse(body, nullptr, false);
        if (parsedBody.is_discarded()) {
            LOG(INFO) << "Generating response for remove request [non JSON]!";

            return response.result(beast::http::status::unprocessable_entity);
        }

        try {
            std::string userName = parsedBody.at("username");
            std::string passwordHashed = parsedBody.at("password");
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

        LOG(INFO) << "Generating response for remove request!";
        return response.version(request->version())
                       .result(beast::http::status::ok)
                       .set(beast::http::field::content_type, "text/plain")
                       .prepare_payload();
    }

    ResponseType handleValidateCreds(asio::ip::tcp::socket& socket, std::shared_ptr<RequestType> request) {
        ResponseWrapper response;

        auto&& tgt = request->target().to_string();

        LOG(INFO) << "Parsing query params";
        LOG(INFO) << "URL is " << tgt;
        try {
            auto queryParams = parseQueryParams(tgt);

            std::string userName = queryParams.at("username");
            std::string password = queryParams.at("password");
            LOG(INFO) << "Username is " << userName << ", password is " << password;
            LOG(INFO) << "Password len is " << password.length();
            if (!service_.credsValid(userName, password))
                return response.result(beast::http::status::not_found);
        }
        catch (std::out_of_range const& err) {
            LOG(ERROR) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
        }
        catch (std::runtime_error const& err) {
            LOG(ERROR) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
        }

        LOG(INFO) << "Users are:";
        for (auto user : service_.getUsers()) {
            LOG(INFO) << user;
        }

        LOG(INFO) << "Generating response for validate creds request!";
        return response.version(request->version())
                       .result(beast::http::status::ok)
                       .set(beast::http::field::content_type, "text/plain")
                       .prepare_payload();
    }

    ResponseType handleGetUsers(asio::ip::tcp::socket& socket, std::shared_ptr<RequestType> request) {
        ResponseWrapper response;

        LOG(INFO) << "Generating response for get users request!";

        json jsonBody = service_.getUsers();
        return response.version(request->version())
                       .result(beast::http::status::ok)
                       .set(beast::http::field::content_type, "text/json")
                       .body(jsonBody.dump())
                       .prepare_payload();
    }

    ResponseType handleUnexpectedRequest(asio::ip::tcp::socket& socket, std::shared_ptr<RequestType> request) {
        ResponseWrapper response;

        LOG(INFO) << "Generating response for unexpected request!";
        return response.version(request->version())
                       .result(beast::http::status::not_found)
                       .prepare_payload();
    }

    ResponseType handleGetNotes(asio::ip::tcp::socket& socket, std::shared_ptr<RequestType> request) {
        ResponseWrapper response;

        auto&& tgt = request->target().to_string();
        json respBody;

        LOG(INFO) << "Parsing query params";
        LOG(INFO) << "URL is " << tgt;
        try {
            auto queryParams = parseQueryParams(tgt);

            std::string userName = queryParams.at("username");
            std::string password = queryParams.at("password");
            LOG(INFO) << "Username is " << userName << ", password is " << password;
            LOG(INFO) << "Password len is " << password.length();

            auto&& notes = service_.getNotesList(userName);
            respBody[userName] = std::vector<std::string>{};

            std::for_each(
                notes.begin(), notes.end(),
                [&respBody, &userName] (auto note) { respBody[userName].push_back(note->getText()); }
            );

            LOG(INFO) << "Printing notes...";
            for (auto note : respBody[userName]) {
                LOG(INFO) << note;
            }
        }
        catch (std::out_of_range const& err) {
            LOG(ERROR) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
        }
        catch (std::runtime_error const& err) {
            LOG(ERROR) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
        }

        LOG(INFO) << "Users are:";
        for (auto user : service_.getUsers()) {
            LOG(INFO) << user;
        }

        LOG(INFO) << "Generating response for get notes request!";
        return response.version(request->version())
                       .result(beast::http::status::ok)
                       .set(beast::http::field::content_type, "text/json")
                       .body(respBody.dump())
                       .prepare_payload();
    }

ResponseType handleDeleteNote(asio::ip::tcp::socket& socket, std::shared_ptr<RequestType> request) {
        ResponseWrapper response;

        auto&& tgt = request->target().to_string();
        LOG(INFO) << "Parsing query params";
        LOG(INFO) << "URL is " << tgt;

        try {
            auto queryParams = parseQueryParams(tgt);

            std::string userName = queryParams.at("username");
            std::string password = queryParams.at("password");
            auto noteNumber = std::stoi(queryParams.at("note"));

            LOG(INFO) << "Username is " << userName << ", password is " << password;
            LOG(INFO) << "Password len is " << password.length();

            service_.removeNote(userName, noteNumber);

            LOG(INFO) << "Printing notes...";
        }
        catch (std::out_of_range const& err) {
            LOG(ERROR) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
        }
        catch (std::runtime_error const& err) {
            LOG(ERROR) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
        }
        catch (std::invalid_argument const& err) {
            LOG(ERROR) << "Error: " << err.what();

            return response.result(beast::http::status::bad_request);
        }

        LOG(INFO) << "Users are:";
        for (auto user : service_.getUsers()) {
            LOG(INFO) << user;
        }

        LOG(INFO) << "Generating response for delete note request!";
        return response.version(request->version())
                       .result(beast::http::status::ok)
                       .set(beast::http::field::content_type, "text/json")
                       .prepare_payload();
    }

    ResponseType handleNoteCreation(asio::ip::tcp::socket& socket, std::shared_ptr<RequestType> request) {
        auto&& body = request->body();
        auto parsedBody = nlohmann::json::parse(body, nullptr, false);
        ResponseWrapper resp;

        if (parsedBody.is_discarded()) {
            LOG(INFO) << "Generating response for note creation request [non JSON]!";

            return resp.result(beast::http::status::unprocessable_entity);
        }

        try {
            std::string userName = parsedBody.at("username");
            std::string passwordHashed = parsedBody.at("password");
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

        LOG(INFO) << "Generating response for note creation request!";
        return resp.version(request->version())
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