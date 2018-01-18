
#pragma once

#ifndef NAIVECOIN_TRANSACTION_SERIALIZE_H
#define NAIVECOIN_TRANSACTION_SERIALIZE_H

#include <string>
#include <list>

#include <naivecoin/transaction/transaction.h>

namespace naivecoin::transaction {

std::string serialize_transactions(std::list<Transaction> const & transactions);
std::list<Transaction> deserialize_transactions(std::string const & text);

} // namespace naivecoin::transaction

#endif // NAIVECOIN_TRANSACTION_SERIALIZE_H
