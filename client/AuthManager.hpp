#include <string>
#include <iostream>

#include <boost/beast.hpp>
#include <glog/logging.h>
#include <nlohmann/json.hpp>

#include "Hasher.hpp"

class AuthManager {
    Hasher hasher_;
public:
    void checkCreds(std::string username, std::string password) {
        std::string target = "/check-user";
        auto hashedPassword = hasher_.hash(password);

        LOG(INFO) << "username is " << username;
        LOG(INFO) << "Password is " << password;

        boost::asio::io_context io;
        boost::beast::tcp_stream stream(io);
        boost::asio::ip::tcp::resolver resolver(io);

        try {
            auto const results = resolver.resolve("localhost", "12345");
            stream.connect(results);
            boost::beast::http::request<boost::beast::http::empty_body> req{
                boost::beast::http::verb::get, target, 11
            };
            // req.body() = nlohmann::json{
            //     { "username", username },
            //     { "password", password },
            // }.dump();

            boost::beast::http::write(stream, req);
            boost::beast::flat_buffer buffer;
            boost::beast::http::response<boost::beast::http::string_body> res;

            boost::beast::http::read(stream, buffer, res);
            std::cout << res.result_int() << std::endl;
        } catch (std::exception const& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
};