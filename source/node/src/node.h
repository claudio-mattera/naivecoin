
#pragma once

#ifndef NAIVECOIN_NODE_NODE_H
#define NAIVECOIN_NODE_NODE_H

#include <random>
#include <list>

#include <spdlog/spdlog.h>

#include <naivecoin/core/block.h>

#include <naivecoin/transaction/transaction.h>
#include <naivecoin/transaction/unspent_output.h>

#include "miner.h"

namespace naivecoin {

class Node
{
public:
    Node(
        std::string const & public_key,
        std::string const & private_key,
        std::vector<std::string> const & peers,
        uint64_t const seed = std::mt19937_64::default_seed
    );

    void start();

    void process_message(std::string const & path, std::string const & message);

private:
    void request_new_block();

    void process_send_block_message(Block const &, std::string const &);
    void process_send_blockchain_message(std::list<Block> const &, std::string const &);
    void process_query_latest_block_message(std::string const &);
    void process_query_blockchain_message(std::string const &);
    void process_unknown_message(std::string const &, std::string const &);
    void process_invalid_message(std::string const &);

private:
    std::vector<std::string> peers;
    std::list<Block> blockchain;
    Miner miner;
    std::thread miner_thread;

    std::shared_ptr<spdlog::logger> logger;
};

} // namespace naivecoin

#endif // NAIVECOIN_NODE_NODE_H
