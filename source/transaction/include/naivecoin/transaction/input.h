
#pragma once

#ifndef NAIVECOIN_TRANSACTION_INPUT_H
#define NAIVECOIN_TRANSACTION_INPUT_H

#include <string>
#include <ostream>
#include <list>
#include <ctime>

namespace naivecoin {

struct Input
{
public:
    std::string const transaction_output_id;
    uint16_t const transaction_output_index;
    std::string const signature;

public:
    Input(
        std::string const & transaction_output_id,
        uint16_t const transaction_output_index,
        std::string const & signature
    );
};

std::ostream & operator<<(std::ostream & stream, Input const & input);

} // namespace naivecoin

#endif // NAIVECOIN_TRANSACTION_INPUT_H
