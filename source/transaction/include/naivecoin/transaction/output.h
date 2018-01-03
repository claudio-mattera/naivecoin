
#pragma once

#ifndef NAIVECOIN_TRANSACTION_OUTPUT_H
#define NAIVECOIN_TRANSACTION_OUTPUT_H

#include <string>
#include <ostream>
#include <list>
#include <ctime>

namespace naivecoin {

struct Output
{
public:
    std::string const address;
    uint16_t const amount;

public:
    Output(
        std::string const address,
        uint64_t const amount
    );
};

std::ostream & operator<<(std::ostream & stream, Output const & output);

} // namespace naivecoin

#endif // NAIVECOIN_TRANSACTION_OUTPUT_H
