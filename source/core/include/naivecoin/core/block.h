
#pragma once

#ifndef NAIVECOIN_CORE_BLOCK_H
#define NAIVECOIN_CORE_BLOCK_H

#include <naivecoin/time/time.h>

#include <string>
#include <ostream>
#include <ctime>

namespace naivecoin::core {

struct Block
{
public:
    uint64_t const index;
    std::string const hash;
    std::string const previous_hash;
    time::instant const timestamp;
    std::string const data;
    uint16_t const difficulty;
    uint64_t const nonce;

    static Block make_block(
        uint64_t const index,
        std::string const previous_hash,
        time::instant const timestamp,
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
        time::instant const timestamp,
        std::string const data,
        uint16_t const difficulty,
        uint64_t const nonce
    );
};

std::ostream & operator<<(std::ostream & stream, Block const & block);

std::string compute_block_hash(
        uint64_t const index,
        std::string const & previous_hash,
        time::instant const & timestamp,
        std::string const & data,
        uint16_t const difficulty,
        uint64_t const nonce
    );

bool is_new_block_valid(Block const & new_block, Block const & previous_block);

template<template<class> class Iterator>
bool is_blockchain_valid(Iterator<Block> const begin, Iterator<Block> const end);

bool hash_matches_difficulty(std::string const & hash, uint16_t const difficulty);

template<template<class> class Iterator>
uint64_t compute_cumulative_difficulty(Iterator<Block> const begin, Iterator<Block> const end);

} // namespace naivecoin::core

#endif // NAIVECOIN_CORE_BLOCK_H
