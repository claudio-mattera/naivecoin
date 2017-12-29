
#pragma once

#ifndef NAIVECOIN_CORE_SERIALIZE_H
#define NAIVECOIN_CORE_SERIALIZE_H

#include <string>
#include <list>

#include <naivecoin/core/block.h>

namespace naivecoin {

std::string serialize_block(Block const & block);
std::string serialize_blockchain(std::list<Block> const & blockchain);

Block deserialize_block(std::string const & text);
std::list<Block> deserialize_blockchain(std::string const & text);

} // namespace naivecoin

#endif // NAIVECOIN_CORE_SERIALIZE_H
