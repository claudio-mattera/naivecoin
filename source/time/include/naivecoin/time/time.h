
#pragma once

#ifndef NAIVECOIN_TIME_TIME_H
#define NAIVECOIN_TIME_TIME_H

#include <string>
#include <ctime>

namespace naivecoin::time {

std::time_t now();
std::time_t parse_timestamp(std::string const & text);
std::string format_timestamp(std::time_t const & timestamp);

} // namespace naivecoin::time

#endif // NAIVECOIN_TIME_TIME_H
