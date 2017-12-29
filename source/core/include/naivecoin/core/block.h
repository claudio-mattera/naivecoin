
#pragma once

#ifndef NAIVECOIN_CORE_BLOCK_H
#define NAIVECOIN_CORE_BLOCK_H

#include <string>
#include <chrono>
#include <ostream>
#include <list>

namespace naivecoin {

struct Block
{
public:
    uint64_t const index;
    std::string const hash;
    std::string const previous_hash;
    std::chrono::system_clock::time_point const timestamp;
    std::string const data;

    static Block make_block(
        uint64_t const index,
        std::string const previous_hash,
        std::chrono::system_clock::time_point const timestamp,
        std::string const data
    );

    static Block genesis();

private:
    Block(
        uint64_t const index,
        std::string const hash,
        std::string const previous_hash,
        std::chrono::system_clock::time_point const timestamp,
        std::string const data
    );
};

std::ostream & operator<<(std::ostream & stream, Block const & block);

std::string compute_hash(
        uint64_t index,
        std::string const & previous_hash,
        std::chrono::system_clock::time_point const & timestamp,
        std::string const & data
    );

bool is_new_block_valid(Block const & new_block, Block const & previous_block);

bool is_blockchain_valid(std::list<Block> const & blockchain);

} // namespace naivecoin

#endif // NAIVECOIN_CORE_BLOCK_H
