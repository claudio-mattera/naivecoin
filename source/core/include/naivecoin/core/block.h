
#pragma once

#ifndef NAIVECOIN_CORE_BLOCK_H
#define NAIVECOIN_CORE_BLOCK_H

#include <string>
#include <chrono>

namespace naivecoin {

struct Block
{
public:
    uint64_t const index;
    std::wstring const hash;
    std::wstring const previous_hash;
    std::chrono::system_clock::time_point const timestamp;
    std::wstring const data;

    static Block make_block(
        uint64_t const index,
        std::wstring const previous_hash,
        std::chrono::system_clock::time_point const timestamp,
        std::wstring const data
    );

    static Block genesis();

private:
    Block(
        uint64_t const index,
        std::wstring const hash,
        std::wstring const previous_hash,
        std::chrono::system_clock::time_point const timestamp,
        std::wstring const data
    );
};

std::wstring compute_hash(
        uint64_t index,
        std::wstring const & previous_hash,
        std::chrono::system_clock::time_point const & timestamp,
        std::wstring const & data
    );

} // namespace naivecoin

#endif // NAIVECOIN_CORE_BLOCK_H
