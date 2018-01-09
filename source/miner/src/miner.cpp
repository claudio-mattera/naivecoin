#include "miner.h"

#include <chrono>
#include <iostream>

#include <naivecoin/core/utils.h>

namespace naivecoin {

Miner::Miner(uint64_t const seed)
: blockchain()
, mutex()
, condition_variable()
, mersenne_twister_engine(seed)
{
    this->blockchain.push_back(naivecoin::Block::genesis());
}

void Miner::start()
{
    while (true) {
        naivecoin::Block const & latest_block = * this->blockchain.crbegin();
        //std::cout << "Latest block: " << latest_block << '\n';

        //std::this_thread::sleep_for(std::chrono::seconds(10));
        this->mine_next_block();
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
    uint16_t const difficulty = this->get_difficulty();

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
    while (true) {
        uint64_t const nonce = this->mersenne_twister_engine();

        std::string const hash = compute_block_hash(
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
        }
    }
}

uint16_t Miner::get_difficulty()
{
    naivecoin::Block const & latest_block = * this->blockchain.crbegin();
    if (latest_block.index % Miner::DIFFICULTY_ADJUSTMENT_INTERVAL_IN_BLOCKS == 0) {
        return this->get_adjusted_difficulty(latest_block);
    } else {
        return latest_block.difficulty;
    }
}

uint16_t Miner::get_adjusted_difficulty(naivecoin::Block const & latest_block)
{
    std::cout << "Adjusting difficulty for block " << latest_block.index;
    auto iterator = this->blockchain.crbegin();
    for (int i = 0; i < Miner::DIFFICULTY_ADJUSTMENT_INTERVAL_IN_BLOCKS; ++i) {
        ++iterator;
    }
    naivecoin::Block const latest_adjustment_block = * iterator;

    auto expected_elapsed_time = std::chrono::seconds(
        Miner::DIFFICULTY_ADJUSTMENT_INTERVAL_IN_BLOCKS * Miner::BLOCK_GENERATION_INTERVAL_IN_SECONDS
    );

    auto actual_elapsed_time = std::chrono::seconds(
        static_cast<int>(
            std::difftime(latest_block.timestamp, latest_adjustment_block.timestamp)
        )
    );

    std::cout << ", elapsed time: " << actual_elapsed_time.count() << "s";
    if (actual_elapsed_time < expected_elapsed_time / 2) {
        std::cout << ", increasing difficulty" << '\n';
        return latest_block.difficulty + 1;
    } else if (actual_elapsed_time > expected_elapsed_time * 2) {
        std::cout << ", decreasing difficulty" << '\n';
        return latest_block.difficulty - 1;
    } else {
        std::cout << ", maintaining difficulty" << '\n';
        return latest_block.difficulty;
    }
}

bool Miner::is_timestamp_valid(naivecoin::Block const & new_block)
{
    naivecoin::Block const & latest_block = * this->blockchain.crbegin();
    std::time_t const now = std::time(nullptr);

    if (std::difftime(new_block.timestamp, latest_block.timestamp) < -60) {
        return false;
    } else if (std::difftime(now, new_block.timestamp) < -60) {
        return false;
    } else {
        return true;
    }
}

} // namespace naivecoin
