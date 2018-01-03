
#pragma once

#ifndef NAIVECOIN_CORE_BLOCK_H
#define NAIVECOIN_CORE_BLOCK_H

#include <string>
#include <ostream>
#include <list>
#include <ctime>

namespace naivecoin {

struct Block
{
public:
    uint64_t const index;
    std::string const hash;
    std::string const previous_hash;
    std::time_t const timestamp;
    std::string const data;
    uint16_t const difficulty;
    uint64_t const nonce;

    static Block make_block(
        uint64_t const index,
        std::string const previous_hash,
        std::time_t const timestamp,
        std::string const data,
        uint16_t const difficulty,
        uint64_t const nonce
    );

    static Block genesis();

private:
    Block(
        uint64_t const index,
        std::string const hash,
        std::string const previous_hash,
        std::time_t const timestamp,
        std::string const data,
        uint16_t const difficulty,
        uint64_t const nonce
    );
};

std::ostream & operator<<(std::ostream & stream, Block const & block);

std::string compute_block_hash(
        uint64_t const index,
        std::string const & previous_hash,
        std::time_t const & timestamp,
        std::string const & data,
        uint16_t const difficulty,
        uint64_t const nonce
    );

bool is_new_block_valid(Block const & new_block, Block const & previous_block);

bool is_blockchain_valid(std::list<Block> const & blockchain);

bool hash_matches_difficulty(std::string const & hash, uint16_t const difficulty);

} // namespace naivecoin

#endif // NAIVECOIN_CORE_BLOCK_H
