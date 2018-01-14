
#pragma once

#ifndef NAIVECOIN_TRANSACTION_TRANSACTION_H
#define NAIVECOIN_TRANSACTION_TRANSACTION_H

#include <string>
#include <ostream>
#include <list>

#include <naivecoin/transaction/output.h>
#include <naivecoin/transaction/unspent_output.h>
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
        std::list<Input> const & inputs,
        std::list<Output> const & outputs
    );
};

std::ostream & operator<<(std::ostream & stream, Transaction const & transaction);

std::string compute_transaction_id(Transaction const & transaction);

std::string compute_input_signature(
    Transaction const & transaction,
    uint64_t const transaction_input_index,
    std::string const & private_key,
    std::list<UnspentOutput> const & unspent_outputs
);

} // namespace naivecoin

#endif // NAIVECOIN_TRANSACTION_TRANSACTION_H
