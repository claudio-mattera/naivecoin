
#pragma once

#ifndef NAIVECOIN_MINER_MINER_H
#define NAIVECOIN_MINER_MINER_H

#include <ctime>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>

#include <boost/asio.hpp>

#include <naivecoin/core/block.h>

namespace naivecoin {

class Miner
{
public:
    Miner();

    void start();

    void request_mine_next_block();

    inline std::list<naivecoin::Block> const & get_blockchain() const
    {
        return this->blockchain;
    }

private:
    enum Operation {
        MINE_NEXT_BLOCK
    };

    uint16_t const BLOCK_GENERATION_INTERVAL_IN_SECONDS = 10;

    uint16_t const DIFFICULTY_ADJUSTMENT_INTERVAL_IN_BLOCKS = 10;

private:
    void mine_next_block();
    naivecoin::Block find_next_block(
        uint64_t const index,
        std::string const & previous_hash,
        std::time_t const & timestamp,
        std::string const & data,
        uint16_t const difficulty
    );
    uint16_t get_difficulty();
    uint16_t get_adjusted_difficulty(naivecoin::Block const & latest_block);
    bool is_timestamp_valid(naivecoin::Block const & new_block);

private:
    std::list<naivecoin::Block> blockchain;

    std::mutex mutex;
    std::condition_variable condition_variable;
};

} // namespace naivecoin

#endif // NAIVECOIN_MINER_MINER_H
