
#pragma once

#ifndef NAIVECOIN_MINER_CONTROLSERVER_H
#define NAIVECOIN_MINER_CONTROLSERVER_H

#include <string>

#include <boost/asio.hpp>

#include "controlconnection.h"
#include "miner.h"

namespace naivecoin {

class control_server
{
public:
    control_server(boost::asio::io_service & io_service, naivecoin::Miner & miner);

private:
    void start_accept();

    void handle_accept(
            control_connection::pointer new_connection,
            const boost::system::error_code & error
        );

    boost::asio::ip::tcp::acceptor acceptor;
    naivecoin::Miner & miner;
};

} // namespace naivecoin

#endif // NAIVECOIN_MINER_CONTROLSERVER_H
