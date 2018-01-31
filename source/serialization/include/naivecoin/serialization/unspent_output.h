
#pragma once

#ifndef NAIVECOIN_SERIALIZATION_UNSPENTOUTPUT_H
#define NAIVECOIN_SERIALIZATION_UNSPENTOUTPUT_H

#include <string>
#include <list>

#include <naivecoin/transaction/unspent_output.h>

namespace naivecoin::serialization {

std::string serialize_unspent_outputs(std::list<naivecoin::transaction::UnspentOutput> const & unspent_outputs);
std::list<naivecoin::transaction::UnspentOutput> deserialize_unspent_outputs(std::string const & text);

} // namespace naivecoin::serialization

#endif // NAIVECOIN_SERIALIZATION_UNSPENTOUTPUT_H
