#include "naivecoin/core/block.h"

#include <sstream>
#include <iomanip>
#include <vector>
#include <ctime>

#include <openssl/sha.h>

namespace naivecoin {

Block::Block(
        uint64_t const index,
        std::wstring const hash,
        std::wstring const previous_hash,
        std::chrono::system_clock::time_point const timestamp,
        std::wstring const data
    )
: index(index)
, hash(hash)
, previous_hash(previous_hash)
, timestamp(timestamp)
, data(data)
{
}

// static
Block Block::make_block(
        uint64_t const index,
        std::wstring const previous_hash,
        std::chrono::system_clock::time_point const timestamp,
        std::wstring const data
    )
{
    std::wstring const hash = compute_hash(
        index,
        previous_hash,
        timestamp,
        data
    );

    return Block(index, hash, previous_hash, timestamp, data);
}

// static
Block Block::genesis()
{
    std::tm tm = {};
    std::stringstream ss("2017-12-28T15:00:00Z");
    ss >> std::get_time(& tm, "%Y-%m-%dT%TZ");
    auto timestamp = std::chrono::system_clock::from_time_t(std::mktime(& tm));

    uint64_t const index = 0;
    std::wstring const previous_hash = L"";
    std::wstring const data = L"";
    std::wstring const hash = compute_hash(
        index,
        previous_hash,
        timestamp,
        data
    );

    return Block(index, hash, previous_hash, timestamp, data);
}

std::wstring compute_hash(
        uint64_t index,
        std::wstring const & previous_hash,
        std::chrono::system_clock::time_point const & timestamp,
        std::wstring const & data
    )
{
    std::wostringstream stream;

    std::time_t const time = std::chrono::system_clock::to_time_t(timestamp);

    stream
        << index
        << previous_hash
        << std::put_time(std::gmtime(& time), L"%FT%TZ")
        << data;

    std::wstring const whole_string = stream.str();

    std::size_t K = 200;
    std::vector<char> temp(K);

    std::size_t n = std::wcstombs(temp.data(), whole_string.c_str(), K);

    unsigned char hash[SHA_DIGEST_LENGTH]; // == 20
    SHA1(reinterpret_cast<unsigned char*>(temp.data()), n, hash);

    wchar_t whash[2 * SHA_DIGEST_LENGTH + 1];

    stream.str(L"");

    for(int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        stream
            << std::setfill(L'0')
            << std::setw(2)
            << std::hex
            << static_cast<unsigned int>(hash[i]);
    }

    return stream.str();
}

} // namespace naivecoin
