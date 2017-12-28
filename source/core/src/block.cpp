#include "naivecoin/core/block.h"

#include <sstream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <iostream>

#include <openssl/sha.h>

namespace naivecoin {

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

    std::wcout << whole_string << L'\n';

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
