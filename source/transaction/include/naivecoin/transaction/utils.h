
#pragma once

#ifndef NAIVECOIN_TRANSACTION_UTILS_H
#define NAIVECOIN_TRANSACTION_UTILS_H

#include <list>

#include <naivecoin/transaction/input.h>
#include <naivecoin/transaction/output.h>

namespace naivecoin::transaction {

std::string join_inputs(std::list<Input> const & inputs);
std::string join_outputs(std::list<Output> const & outputs);

} // namespace naivecoin::transaction

#endif // NAIVECOIN_TRANSACTION_UTILS_H
