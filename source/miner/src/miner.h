
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

private:
    void mine_next_block();
    naivecoin::Block find_next_block(
        uint64_t const index,
        std::string const & previous_hash,
        std::time_t const & timestamp,
        std::string const & data,
        uint16_t const difficulty
    );

private:
    std::list<naivecoin::Block> blockchain;

    std::mutex mutex;
    std::condition_variable condition_variable;
};

} // namespace naivecoin

#endif // NAIVECOIN_MINER_MINER_H
