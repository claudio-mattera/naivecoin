
#pragma once

#ifndef NAIVECOIN_MINER_CONTROLCONNECTION_H
#define NAIVECOIN_MINER_CONTROLCONNECTION_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/asio.hpp>

#include <spdlog/spdlog.h>

#include "miner.h"

namespace naivecoin {

class control_connection
    : public boost::enable_shared_from_this<control_connection>
{
public:
    typedef boost::shared_ptr<control_connection> pointer;

    static pointer create(boost::asio::io_service & io_service);

    boost::asio::ip::tcp::socket & socket();

    void start(naivecoin::Miner & miner);

private:
    control_connection(boost::asio::io_service & io_service);

    void send_response(std::string const & text);

    void handle_write(
            const boost::system::error_code & /*error*/,
            size_t /*bytes_transferred*/
        );

    boost::asio::ip::tcp::socket connection_socket;
    std::shared_ptr<spdlog::logger> logger;
};

} // namespace naivecoin

#endif // NAIVECOIN_MINER_CONTROLCONNECTION_H
