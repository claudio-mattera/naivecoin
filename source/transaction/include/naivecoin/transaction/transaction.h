
#pragma once

#ifndef NAIVECOIN_TRANSACTION_TRANSACTION_H
#define NAIVECOIN_TRANSACTION_TRANSACTION_H

#include <string>
#include <ostream>
#include <list>

#include <naivecoin/transaction/output.h>
#include <naivecoin/transaction/unspent_output.h>
#include <naivecoin/transaction/input.h>

namespace naivecoin::transaction {

uint64_t const COINBASE_AMOUNT = 50;

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

std::string compute_transaction_id(Transaction const & transaction);

std::string compute_input_signature(
    Transaction const & transaction,
    uint64_t const transaction_input_index,
    std::string const & private_key,
    std::list<UnspentOutput> const & unspent_outputs
);

bool is_transaction_list_valid(
    uint64_t const index,
    std::list<Transaction> const & transactions,
    std::list<UnspentOutput> const & unspent_outputs
);

bool is_transaction_valid(
    Transaction const & transaction,
    std::list<UnspentOutput> const & unspent_outputs
);

bool is_coinbase_transaction_valid(
    uint64_t const index,
    Transaction const & coinbase_transaction
);

Transaction create_transaction(
    std::list<Input> inputs,
    std::list<Output> const & outputs,
    std::list<std::string> const & private_keys,
    std::list<UnspentOutput> const & unspent_outputs
);

Transaction create_coinbase_transaction(
    uint64_t const index,
    std::string const & address
);

std::list<UnspentOutput> update_unspent_outputs(
    uint64_t  const index,
    std::list<Transaction> const & new_transactions,
    std::list<UnspentOutput> old_unspent_outputs
);

} // namespace naivecoin::transaction

#endif // NAIVECOIN_TRANSACTION_TRANSACTION_H
