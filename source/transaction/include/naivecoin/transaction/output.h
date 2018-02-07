
#pragma once

#ifndef NAIVECOIN_TRANSACTION_OUTPUT_H
#define NAIVECOIN_TRANSACTION_OUTPUT_H

#include <string>
#include <ostream>
#include <list>
#include <ctime>

namespace naivecoin::transaction {

struct Output
{
public:
    std::string const address;
    uint64_t const amount;

public:
    Output(
        std::string const address,
        uint64_t const amount
    );
};

std::ostream & operator<<(std::ostream & stream, Output const & output);

} // namespace naivecoin::transaction

#endif // NAIVECOIN_TRANSACTION_OUTPUT_H
