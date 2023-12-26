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
    asio::ip::tcp::socket socket_;
    Handler handler_;

public:
    explicit Server(unsigned port)
        : port_{port}, acceptor{io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)}, socket_{io_context} {}

    void start_ctx() { io_context.run(); }

    void start() {
        LOG(INFO) << "Accepting on " << port_;
        acceptor.async_accept(socket_, [this] (auto err) {
            LOG(INFO) << "Accepted!";

            auto sess = std::make_shared<Session>(std::move(socket_), handler_, io_context);
            sess->handle();

            start();
        });
    }
};