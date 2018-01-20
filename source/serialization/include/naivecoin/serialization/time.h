
#pragma once

#ifndef NAIVECOIN_SERIALIZATION_TIME_H
#define NAIVECOIN_SERIALIZATION_TIME_H

#include <naivecoin/core/block.h>

namespace naivecoin::serialization {

std::time_t now();
std::time_t parse_timestamp(std::string const & text);
std::string format_timestamp(std::time_t const & timestamp);

} // namespace naivecoin::serialization

#endif // NAIVECOIN_SERIALIZATION_TIME_H
