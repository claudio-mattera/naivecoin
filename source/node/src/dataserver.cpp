#include "dataserver.h"

#include <iostream>

#include <boost/bind.hpp>

#include <naivecoin/core/serialize.h>

namespace naivecoin {

data_server::data_server(
    std::function<void(std::string const &)> const & message_handler,
    boost::asio::io_service & io_service,
    uint64_t port
)
: acceptor(
    io_service,
    boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)
)
, message_handler(message_handler)
, logger(spdlog::get("dataserver"))
{
    start_accept();
}

void data_server::start_accept()
{
    data_connection::pointer new_connection =
        data_connection::create(this->message_handler, this->acceptor.get_io_service());

    this->logger->debug("Calling async_accept");
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
    this->logger->debug("Handling accept");
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
    using boost::asio::ip::tcp;

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(hostname, std::to_string(port));
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);

    std::string message = create_send_block_message(block, "");

    boost::system::error_code ignored_error;
    boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
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
