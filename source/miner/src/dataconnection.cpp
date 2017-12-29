#include "dataconnection.h"

#include <boost/bind.hpp>
#include <boost/array.hpp>

#include <iostream>

#include <ctime>

namespace {

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(& now);
}

std::string get_path(std::string const & request)
{
    int const get_pos = request.find("GET ") + 4;
    int const end_pos = request.find(" HTTP", get_pos);
    return request.substr(get_pos, end_pos - get_pos);
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
        std::exit(0);

    } else {
        this->message = make_daytime_string();

        boost::asio::async_write(
            this->connection_socket,
            boost::asio::buffer(this->message),
            boost::bind(
                & data_connection::handle_write,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
            )
        );
    }
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
