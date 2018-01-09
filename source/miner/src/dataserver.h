
#pragma once

#ifndef NAIVECOIN_MINER_DATASERVER_H
#define NAIVECOIN_MINER_DATASERVER_H

#include <string>
#include <list>

#include <boost/asio.hpp>

#include <naivecoin/core/block.h>

#include "dataconnection.h"
#include "miner.h"

namespace naivecoin {

class data_server
{
public:
    data_server(boost::asio::io_service & io_service, uint64_t port, naivecoin::Miner & miner);

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
    naivecoin::Miner & miner;
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

#endif // NAIVECOIN_MINER_DATASERVER_H
