
#pragma once

#ifndef NAIVECOIN_CORE_UTILS_H
#define NAIVECOIN_CORE_UTILS_H

#include <string>
#include <array>
#include <ctime>

namespace naivecoin {

std::time_t now();
std::time_t parse_timestamp(std::string const & text);
std::string format_timestamp(std::time_t const & timestamp);

template <int N>
std::array<bool, 4 * N> hex_to_binary(std::string const & text)
{
    std::array<bool, 4 * N> binary;

    for (int i = 0; i < N; ++i) {
        char const digit = text[i];
        std::array<char, 2> const s{digit, 0};
        uint8_t number = std::strtoul(s.data(), nullptr, 16);
        binary[4*i + 3] = number % 2;
        number /= 2;
        binary[4*i + 2] = number % 2;
        number /= 2;
        binary[4*i + 1] = number % 2;
        number /= 2;
        binary[4*i] = number % 2;
    }

    return binary;
}

} // namespace naivecoin

#endif // NAIVECOIN_CORE_UTILS_H
