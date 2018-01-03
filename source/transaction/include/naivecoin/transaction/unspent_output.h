
#pragma once

#ifndef NAIVECOIN_TRANSACTION_UNSPENTOUTPUT_H
#define NAIVECOIN_TRANSACTION_UNSPENTOUTPUT_H

#include <string>
#include <ostream>
#include <list>
#include <ctime>

namespace naivecoin {

struct UnspentOutput
{
public:
    std::string const transaction_id;
    uint64_t const transaction_index;
    std::string const address;
    uint64_t const amount;

public:
    UnspentOutput(
        std::string const & transaction_id,
        uint64_t const transaction_index,
        std::string const & address,
        uint64_t const amount
    );
};

std::ostream & operator<<(std::ostream & stream, UnspentOutput const & unspent_output);

} // namespace naivecoin

#endif // NAIVECOIN_TRANSACTION_UNSPENTOUTPUT_H
