#include "dataconnection.h"

#include <naivecoin/core/serialize.h>
#include <naivecoin/http/http.h>

#include <boost/bind.hpp>
#include <boost/array.hpp>

#include <ctime>

namespace {

std::string read_whole_payload(boost::asio::ip::tcp::socket & connection_socket)
{
    std::ostringstream stream;

    boost::array<char, 128> buf;
    boost::system::error_code error;

    while (true) {
        if (stream.tellp() > 0 && connection_socket.available(error) <= 0) {
            break;
        }
        size_t const len = connection_socket.read_some(boost::asio::buffer(buf), error);

        stream.write(buf.data(), len);

        if (error == boost::asio::error::eof) {
            break; // Connection closed cleanly by peer.
        } else if (error) {
            throw boost::system::system_error(error);
        }
    }

    return stream.str();
}

} // unnamed namespace


namespace naivecoin {

//static
data_connection::pointer data_connection::create(
    std::function<void(std::string const &)> const & message_handler,
    boost::asio::io_service & io_service
)
{
    return pointer(new data_connection(message_handler, io_service));
}

boost::asio::ip::tcp::socket & data_connection::socket()
{
    return this->connection_socket;
}

void data_connection::start()
{
    this->logger->debug("Responding");

    std::string const request = read_whole_payload(this->connection_socket);

    std::string const response = process_request(
        request,
        [this](Method const /*method*/, std::string const & /*path*/, std::string const & data) {
            this->message_handler(data);
            return "";
        }
    );

    boost::asio::async_write(
        this->connection_socket,
        boost::asio::buffer(response),
        boost::bind(
            & data_connection::handle_write,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred
        )
    );
}

data_connection::data_connection(
    std::function<void(std::string const &)> const & message_handler,
    boost::asio::io_service & io_service
)
: message_handler(message_handler)
, connection_socket(io_service)
, logger(spdlog::get("dataconnection"))
{
}

void data_connection::handle_write(
        const boost::system::error_code & /*error*/,
        size_t /*bytes_transferred*/
    )
{
}

} // namespace naivecoin
