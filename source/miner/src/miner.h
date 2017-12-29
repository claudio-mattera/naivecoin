
#pragma once

#ifndef NAIVECOIN_MINER_MINER_H
#define NAIVECOIN_MINER_MINER_H

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

private:
    enum Operation {
        MINE_NEXT_BLOCK
    };

private:
    void mine_next_block();

private:
    std::list<naivecoin::Block> blockchain;

    std::mutex mutex;
    std::condition_variable condition_variable;
};

} // namespace naivecoin

#endif // NAIVECOIN_MINER_MINER_H
