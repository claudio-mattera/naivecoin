
#pragma once

#ifndef NAIVECOIN_CORE_UTILS_H
#define NAIVECOIN_CORE_UTILS_H

#include <string>
#include <ctime>

namespace naivecoin {

std::time_t now();
std::time_t parse_timestamp(std::string const & text);
std::string format_timestamp(std::time_t const & timestamp);

} // namespace naivecoin

#endif // NAIVECOIN_CORE_UTILS_H
