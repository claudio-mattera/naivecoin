#include "controlconnection.h"

#include <boost/bind.hpp>
#include <boost/array.hpp>

#include <ctime>

#include <naivecoin/core/serialize.h>

namespace {

std::string get_path(std::string const & request)
{
    int const get_pos = request.find("GET ") + 4;
    int const end_pos = request.find(" HTTP", get_pos);
    return request.substr(get_pos, end_pos - get_pos);
}

std::string make_success_response(std::string const & data = "")
{
    return "HTTP/1.1 200 OK\n\n" + data;
}

std::string make_error_response(std::string const & data = "")
{
    return "HTTP/1.1 400 Bad Request\n\n" + data;
}

} // unnamed namespace


namespace naivecoin {

//static
control_connection::pointer control_connection::create(boost::asio::io_service & io_service)
{
    return pointer(new control_connection(io_service));
}

boost::asio::ip::tcp::socket & control_connection::socket()
{
    return this->connection_socket;
}

void control_connection::start(naivecoin::Miner & miner)
{
    this->logger->info("Responding");

    boost::array<char, 128> buf;
    boost::system::error_code error;
    size_t len = this->connection_socket.read_some(boost::asio::buffer(buf), error);

    if (error == boost::asio::error::eof) {
        return; // Connection closed cleanly by peer.
    } else if (error) {
        throw boost::system::system_error(error);
    }

    std::ostringstream stream;
    stream.write(buf.data(), len);
    std::string path = get_path(stream.str());

    if (path == "/quit") {
        this->send_response(make_success_response());
        std::exit(0);
    } else if (path == "/blockchain") {
        auto blockchain = miner.get_blockchain();
        auto data = naivecoin::serialize_blockchain(blockchain);
        std::string const text = make_success_response(data);
        this->send_response(text);
    } else {
        this->send_response(make_error_response());
    }
}

control_connection::control_connection(boost::asio::io_service & io_service)
: connection_socket(io_service)
, logger(spdlog::get("controlconnection"))
{
}

void control_connection::handle_write(
        const boost::system::error_code & /*error*/,
        size_t /*bytes_transferred*/
    )
{
}

void control_connection::send_response(std::string const & text)
{
    boost::asio::async_write(
        this->connection_socket,
        boost::asio::buffer(text),
        boost::bind(
            & control_connection::handle_write,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred
        )
    );
}

} // namespace naivecoin
