
#pragma once

#ifndef NAIVECOIN_TRANSACTION_TRANSACTION_H
#define NAIVECOIN_TRANSACTION_TRANSACTION_H

#include <string>
#include <ostream>
#include <list>

#include <naivecoin/transaction/output.h>
#include <naivecoin/transaction/input.h>

namespace naivecoin {

struct Transaction
{
public:
    std::string const id;
    std::list<Input> const inputs;
    std::list<Output> const outputs;

public:
    Transaction(
        std::string const & id,
        std::list<Input> const & inputs,
        std::list<Output> const & outputs
    );
};

std::ostream & operator<<(std::ostream & stream, Transaction const & transaction);

} // namespace naivecoin

#endif // NAIVECOIN_TRANSACTION_TRANSACTION_H
