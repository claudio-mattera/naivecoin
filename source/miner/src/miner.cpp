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
    std::time_t const timestamp= naivecoin::now();
    uint16_t const difficulty = 0;

    naivecoin::Block const next_block = this->find_next_block(
        index,
        previous_hash,
        timestamp,
        data,
        difficulty
    );

    this->blockchain.push_back(next_block);
}

naivecoin::Block Miner::find_next_block(
    uint64_t const index,
    std::string const & previous_hash,
    std::time_t const & timestamp,
    std::string const & data,
    uint16_t const difficulty
)
{
    uint64_t nonce = 0;
    while (true) {
        std::string const hash = compute_hash(
            index,
            previous_hash,
            timestamp,
            data,
            difficulty,
            nonce
        );
        if (naivecoin::hash_matches_difficulty(hash, difficulty)) {
            return naivecoin::Block::make_block(
                index,
                previous_hash,
                timestamp,
                data,
                difficulty,
                nonce
            );
        } else {
            ++nonce;
        }
    }
}

} // namespace naivecoin
