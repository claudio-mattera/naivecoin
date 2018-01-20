
#pragma once

#ifndef NAIVECOIN_CORE_UTILS_H
#define NAIVECOIN_CORE_UTILS_H

#include <string>
#include <array>

namespace naivecoin::core {

template <int N>
std::array<bool, 4 * N> hex_to_binary(std::string const & text);

std::string replace(std::string text, std::string const & pattern, std::string const & after);

} // namespace naivecoin::core

#endif // NAIVECOIN_CORE_UTILS_H
