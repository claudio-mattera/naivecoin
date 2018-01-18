
#pragma once

#ifndef NAIVECOIN_NODE_MINER_H
#define NAIVECOIN_NODE_MINER_H

#include <ctime>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <random>

#include <boost/asio.hpp>

#include <spdlog/spdlog.h>

#include <naivecoin/core/block.h>

namespace naivecoin::node {

using namespace naivecoin;

class Miner
{
public:
    Miner(
        std::string const & public_key,
        uint64_t const sleep_time,
        uint64_t const seed = std::mt19937_64::default_seed
    );

    void start();

    void request_mine_next_block(core::Block const & latest_block);

    core::Block get_next_block();

private:
    uint16_t const BLOCK_GENERATION_INTERVAL_IN_SECONDS = 10;

    uint16_t const DIFFICULTY_ADJUSTMENT_INTERVAL_IN_BLOCKS = 10;

private:
    core::Block mine_next_block(core::Block const & latest_block);
    core::Block find_next_block(
        uint64_t const index,
        std::string const & previous_hash,
        std::time_t const & timestamp,
        std::string const & data,
        uint16_t const difficulty
    );
    uint16_t get_difficulty();
    uint16_t get_adjusted_difficulty(core::Block const & latest_block);
    bool is_timestamp_valid(core::Block const & new_block);

private:
    std::string const public_key;
    uint64_t const sleep_time;

    std::queue<core::Block> latest_blocks;
    std::queue<core::Block> next_blocks;

    std::mutex input_mutex;
    std::condition_variable input_condition_variable;

    std::mutex output_mutex;
    std::condition_variable output_condition_variable;

    std::mt19937_64 mersenne_twister_engine;

    std::shared_ptr<spdlog::logger> logger;
};

} // namespace naivecoin::node

#endif // NAIVECOIN_NODE_MINER_H
