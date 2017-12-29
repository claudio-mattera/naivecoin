#include "dataserver.h"

#include <iostream>

#include <boost/bind.hpp>



namespace naivecoin {

data_server::data_server(boost::asio::io_service & io_service)
: acceptor(
    io_service,
    boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8014)
)
{
    start_accept();
}

void data_server::start_accept()
{
    data_connection::pointer new_connection =
        data_connection::create(this->acceptor.get_io_service());

    std::cout << "Calling async_accept" << '\n';
    this->acceptor.async_accept(
        new_connection->socket(),
        boost::bind(
            & data_server::handle_accept,
            this,
            new_connection,
            boost::asio::placeholders::error
        )
    );
}

void data_server::handle_accept(
        data_connection::pointer new_connection,
        const boost::system::error_code & error
    )
{
    std::cout << "Handling accept" << '\n';
    if (!error)
    {
        new_connection->start();
    }

    start_accept();
}

void send_block(
    naivecoin::Block const & block,
    std::string const & hostname,
    int const port
)
{

}

void send_blockchain(
    std::list<naivecoin::Block> const & blockchain,
    std::string const & hostname,
    int const port
)
{

}

naivecoin::Block request_latest_block(
    std::string const & hostname,
    int const port
)
{

}

std::list<naivecoin::Block> request_blockchain(
    std::string const & hostname,
    int const port
)
{

}

} // namespace naivecoin
