
#pragma once

#ifndef NAIVECOIN_SERIALIZATION_TRANSACTION_H
#define NAIVECOIN_SERIALIZATION_TRANSACTION_H

#include <string>
#include <list>

#include <naivecoin/transaction/transaction.h>

namespace naivecoin::serialization {

std::string serialize_transaction(naivecoin::transaction::Transaction const & transaction);
naivecoin::transaction::Transaction deserialize_transaction(std::string const & text);

std::string serialize_transactions(std::list<naivecoin::transaction::Transaction> const & transactions);
std::list<naivecoin::transaction::Transaction> deserialize_transactions(std::string const & text);

} // namespace naivecoin::serialization

#endif // NAIVECOIN_SERIALIZATION_TRANSACTION_H
