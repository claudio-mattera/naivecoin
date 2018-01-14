
#pragma once

#ifndef NAIVECOIN_NODE_DATACONNECTION_H
#define NAIVECOIN_NODE_DATACONNECTION_H

#include <functional>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/asio.hpp>

#include <spdlog/spdlog.h>

namespace naivecoin {

class data_connection
    : public boost::enable_shared_from_this<data_connection>
{
public:
    typedef boost::shared_ptr<data_connection> pointer;

    static pointer create(
        std::function<void(std::string const &)> const & message_handler,
        boost::asio::io_service & io_service
    );

    boost::asio::ip::tcp::socket & socket();

    void start();

private:
    data_connection(
        std::function<void(std::string const &)> const & message_handler,
        boost::asio::io_service & io_service
    );

    void handle_write(
            const boost::system::error_code & /*error*/,
            size_t /*bytes_transferred*/
        );

    std::function<void(std::string const &)> const & message_handler;
    boost::asio::ip::tcp::socket connection_socket;
    std::shared_ptr<spdlog::logger> logger;
};

} // namespace naivecoin

#endif // NAIVECOIN_NODE_DATACONNECTION_H