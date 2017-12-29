#include "miner.h"

#include <chrono>
#include <iostream>

#include <naivecoin/core/utils.h>

namespace naivecoin {

Miner::Miner()
{
    this->blockchain.push_back(naivecoin::Block::genesis());
}

void Miner::start()
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        std::cout << "Haha" << '\n';
    }
}

void Miner::request_mine_next_block()
{

}

void Miner::mine_next_block()
{
    naivecoin::Block const & latest_block = * this->blockchain.crbegin();

    uint64_t const index = 1 + latest_block.index;
    std::string const & previous_hash = latest_block.hash;
    std::string const data;
    auto timestamp = naivecoin::now();
    naivecoin::Block const next_block = naivecoin::Block::make_block(
        index,
        previous_hash,
        timestamp,
        data
    );

    this->blockchain.push_back(next_block);
}

} // namespace naivecoin
