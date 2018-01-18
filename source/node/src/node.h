
#pragma once

#ifndef NAIVECOIN_NODE_NODE_H
#define NAIVECOIN_NODE_NODE_H

#include <random>
#include <list>

#include <spdlog/spdlog.h>

#include <simple-web-server/server_http.hpp>

#include <naivecoin/core/block.h>

#include <naivecoin/transaction/transaction.h>
#include <naivecoin/transaction/unspent_output.h>

#include "miner.h"
#include "sender.h"

namespace naivecoin::node {

using namespace naivecoin;

class Node
{
public:
    Node(
        uint16_t const port,
        std::string const & public_key,
        std::vector<std::string> const & peers,
        uint64_t const sleep_time,
        uint64_t const seed = std::mt19937_64::default_seed
    );

    void start();

private:
    void connect_to_peer(std::string const & peer);
    void send_block_to_peers(core::Block const & block);
    bool try_adding_block_to_blockchain(core::Block const & block);
    void replace_blockchain(std::list<core::Block> blockchain);
    core::Block get_latest_block() const;
    std::string serialize_blockchain() const;
    uint64_t compute_cumulative_difficulty() const;
    std::string create_send_blockchain_message(std::string const & address) const;

    void process_send_block_message(core::Block const &, std::string const &);
    void process_send_blockchain_message(std::list<core::Block> const &, std::string const &);
    void process_query_latest_block_message(std::string const &);
    void process_query_blockchain_message(std::string const &);
    void process_unknown_message(std::string const &, std::string const &);
    void process_invalid_message(std::string const &, std::string const &);

    void add_peer(std::string const & peer);

private:
    std::string const address;
    std::vector<std::string> peers;
    std::list<core::Block> blockchain;
    Miner miner;
    Sender sender;
    std::thread miner_thread;
    std::thread server_thread;
    std::thread sender_thread;

    SimpleWeb::Server<SimpleWeb::HTTP> server;

    std::shared_ptr<spdlog::logger> logger;
};

} // namespace naivecoin::node

#endif // NAIVECOIN_NODE_NODE_H
