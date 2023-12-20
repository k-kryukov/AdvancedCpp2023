#include <iostream>
#include <string>
#include <utility>

#include <glog/logging.h>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

namespace http = boost::beast::http;
namespace beast = boost::beast;
namespace net = boost::asio;

using tcp = boost::asio::ip::tcp;

/*
 * This test code is deprecated
 */
int main() {
  try {
    net::io_context ioc;
    std::string body;
    int status_code;

    tcp::acceptor acceptor(ioc, {tcp::v4(), 1 << 13});

    while (true) {
      tcp::socket socket(ioc);

      acceptor.accept(socket);

      beast::flat_buffer buffer;
      http::request<http::string_body> request;

      http::read(socket, buffer, request);

      if (request.target() == "/save-body") {
        body = request.body().data();
        std::cout << "Saved: " << body << std::endl;

        http::response<http::string_body> response;
        response.version(request.version());
        response.result(http::status::ok);
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "application/json");
        response.keep_alive(request.keep_alive());
        response.body() = "Saved: " + body + "\n";
        response.prepare_payload();

        http::write(socket, response);
      } else if (request.target() == "/save-status") {
        http::response<http::string_body> response;

        try {
          status_code = std::stoi(request.body());
        } catch (std::exception const& e) {
          response.result(http::status::bad_request);
          http::write(socket, response);
          continue;
        }
        LOG(INFO) << "Saving status...";

        response.version(request.version());
        response.result(http::status::ok);
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.keep_alive(request.keep_alive());
        response.body() = "Saved: " + body + "\n";
        response.prepare_payload();
        http::write(socket, response);
      } else {
        LOG(INFO) << "Replying with prepared data, code: " << status_code;
        http::response<http::string_body> response;
        response.version(request.version());
        response.result(status_code);
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "application/json");
        response.keep_alive(request.keep_alive());
        response.body() = body;
        response.prepare_payload();

        http::write(socket, response);
      }

      socket.shutdown(tcp::socket::shutdown_send);
    }
  } catch (std::exception const& e) {
    std::cerr << "Ошибка: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}