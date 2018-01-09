#include "dataconnection.h"

#include <naivecoin/core/serialize.h>
#include <naivecoin/http/http.h>

#include <boost/bind.hpp>
#include <boost/array.hpp>

#include <iostream>

#include <ctime>

namespace {

std::string read_whole_payload(boost::asio::ip::tcp::socket & connection_socket)
{
    std::ostringstream stream;

    boost::array<char, 128> buf;
    boost::system::error_code error;

    while (true) {
        if (connection_socket.available(error) <= 0) {
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
data_connection::pointer data_connection::create(boost::asio::io_service & io_service)
{
    return pointer(new data_connection(io_service));
}

boost::asio::ip::tcp::socket & data_connection::socket()
{
    return this->connection_socket;
}

void data_connection::start()
{
    std::cout << "Responding" << '\n';

    std::string const request = read_whole_payload(this->connection_socket);

    std::string const response = process_request(
        request,
        [](Method const method, std::string const & path, std::string const & data) {
            int const prefix = data[0] - '0';
            std::string const rest_of_data = data.substr(1, -1);
            std::cout << "Received prefix: " << prefix << std::endl;

            switch (prefix) {
                case 1:
                {
                    Block const block = deserialize_block(rest_of_data);
                    std::cout << "Received block " << block << std::endl;
                    break;
                }
                default:
                {
                    std::cout << "Prefix: " << prefix << '\n';
                    break;
                }
            }
            return "Some text";
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

data_connection::data_connection(boost::asio::io_service & io_service)
: connection_socket(io_service)
{
}

void data_connection::handle_write(
        const boost::system::error_code & /*error*/,
        size_t /*bytes_transferred*/
    )
{
}

} // namespace naivecoin
