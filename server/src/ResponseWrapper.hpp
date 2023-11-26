#include <utility>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <glog/logging.h>
#include <nlohmann/json.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;

using namespace nlohmann;

class ResponseWrapper {
    beast::http::response<beast::http::string_body> resp;

public:
    ResponseWrapper& version(unsigned version) { resp.version(version); return *this; }
    ResponseWrapper& result(beast::http::status status) { resp.result(status); return *this; }
    ResponseWrapper& set(beast::http::field header, std::string_view val) { resp.set(header, val.data()); return *this; }
    ResponseWrapper& body(std::string body) { resp.body() = std::move(body); return *this; }
    ResponseWrapper& prepare_payload() { resp.prepare_payload(); return *this; }

    operator decltype(resp)() { return resp; }

    auto get() { return resp ;}
};