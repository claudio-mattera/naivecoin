
#pragma once

#ifndef NAIVECOIN_TIME_TIME_H
#define NAIVECOIN_TIME_TIME_H

#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>
#include <ctime>

namespace naivecoin::time {

typedef boost::posix_time::ptime instant;

instant now();
instant parse_timestamp(std::string const & text);
std::string format_timestamp(instant const & timestamp);
int difference_in_seconds(instant const & first, instant const & second);

} // namespace naivecoin::time

#endif // NAIVECOIN_TIME_TIME_H
