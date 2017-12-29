
#pragma once

#ifndef NAIVECOIN_CORE_SERIALIZE_H
#define NAIVECOIN_CORE_SERIALIZE_H

#include <string>
#include <list>

#include <naivecoin/core/block.h>

namespace naivecoin {

std::wstring serialize_block(Block const & block);
std::wstring serialize_blockchain(std::list<Block> const & blockchain);

Block deserialize_block(std::wstring const & text);
std::list<Block> deserialize_blockchain(std::wstring const & text);

} // namespace naivecoin

#endif // NAIVECOIN_CORE_SERIALIZE_H
