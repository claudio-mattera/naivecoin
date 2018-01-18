
#pragma once

#ifndef NAIVECOIN_CORE_UTILS_H
#define NAIVECOIN_CORE_UTILS_H

#include <string>
#include <array>
#include <ctime>

namespace naivecoin::core {

std::time_t now();
std::time_t parse_timestamp(std::string const & text);
std::string format_timestamp(std::time_t const & timestamp);

template <int N>
std::array<bool, 4 * N> hex_to_binary(std::string const & text);

} // namespace naivecoin::core

#endif // NAIVECOIN_CORE_UTILS_H
