
#pragma once

#ifndef NAIVECOIN_NODE_DATASERVER_H
#define NAIVECOIN_NODE_DATASERVER_H

#include <string>
#include <list>
#include <functional>

#include <boost/asio.hpp>

#include <naivecoin/core/block.h>

#include <spdlog/spdlog.h>

#include "messagehandler.h"
#include "dataconnection.h"

namespace naivecoin {

class data_server
{
public:
    data_server(
        MessageHandler const & message_handler,
        boost::asio::io_service & io_service,
        uint64_t port
    );

    enum Prefix {
        Block = 0x01,
        Blockchain = 0x02,
        RequestLatestBlock = 0x03,
        RequestBlockchain = 0x04
    };

private:
    void start_accept();

    void handle_accept(
            data_connection::pointer new_connection,
            const boost::system::error_code & error
        );

    boost::asio::ip::tcp::acceptor acceptor;
    MessageHandler const & message_handler;
    std::shared_ptr<spdlog::logger> logger;
};

void send_block(
    naivecoin::Block const & block,
    std::string const & hostname,
    int const port
);

void send_blockchain(
    std::list<naivecoin::Block> const & blockchain,
    std::string const & hostname,
    int const port
);

naivecoin::Block request_latest_block(
    std::string const & hostname,
    int const port
);

std::list<naivecoin::Block> request_blockchain(
    std::string const & hostname,
    int const port
);

} // namespace naivecoin

#endif // NAIVECOIN_NODE_DATASERVER_H
