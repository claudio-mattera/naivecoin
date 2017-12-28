#include "controlserver.h"

#include <iostream>

#include <boost/bind.hpp>

namespace naivecoin {

control_server::control_server(boost::asio::io_service & io_service)
: acceptor(
    io_service,
    boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8013)
)
{
    start_accept();
}

void control_server::start_accept()
{
    control_connection::pointer new_connection =
        control_connection::create(this->acceptor.get_io_service());

    std::cout << "Calling async_accept" << '\n';
    this->acceptor.async_accept(
        new_connection->socket(),
        boost::bind(
            & control_server::handle_accept,
            this,
            new_connection,
            boost::asio::placeholders::error
        )
    );
}

void control_server::handle_accept(
        control_connection::pointer new_connection,
        const boost::system::error_code& error
    )
{
    std::cout << "Handling accept" << '\n';
    if (!error)
    {
        new_connection->start();
    }

    start_accept();
}

} // namespace naivecoin
