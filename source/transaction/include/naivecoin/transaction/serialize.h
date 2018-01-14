
#pragma once

#ifndef NAIVECOIN_TRANSACTION_SERIALIZE_H
#define NAIVECOIN_TRANSACTION_SERIALIZE_H

#include <string>
#include <list>

#include <naivecoin/transaction/transaction.h>

namespace naivecoin {

std::string serialize_transactions(std::list<Transaction> const & transactions);
std::list<Transaction> deserialize_transactions(std::string const & text);

} // namespace naivecoin

#endif // NAIVECOIN_TRANSACTION_SERIALIZE_H
