#include <utility>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <variant>

namespace asio = boost::asio;
namespace beast = boost::beast;

class Server {
    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor;
public:
    explicit Server(unsigned port) : acceptor{io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)} {}

    void start() {
        while (true) {
            // Создаем сокет и ждем входящее соединение
            asio::ip::tcp::socket socket(io_context);

            LOG(INFO) << "Accepting...";
            acceptor.accept(socket);
            LOG(INFO) << "Accepted!";

            // Читаем данные из сокета
            LOG(INFO) << "Reading request!";
            beast::flat_buffer buffer;
            beast::http::request<beast::http::string_body> request;
            beast::http::read(socket, buffer, request);

            // auto fields = request.at("Content-Type");
            // LOG(INFO) << "Fields are " << fields;
            // auto body = request.body();
            // auto x = nlohmann::json::parse(body);

            // LOG(INFO) << "Body is " << x;
            // LOG(INFO) << "Header is " << request.keep_alive();

            // Формируем ответ
            LOG(INFO) << "Generating response!";
            beast::http::response<beast::http::string_body> response;
            response.version(request.version());
            response.result(beast::http::status::ok);
            response.set(beast::http::field::content_type, "text/plain");
            response.body() = request.body();
            response.prepare_payload();

            // Отправляем ответ
            beast::http::write(socket, response);

            // Закрываем соединение
            socket.shutdown(asio::ip::tcp::socket::shutdown_send);
        }
    }
};