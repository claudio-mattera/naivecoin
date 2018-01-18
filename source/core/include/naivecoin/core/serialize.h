
#pragma once

#ifndef NAIVECOIN_CORE_SERIALIZE_H
#define NAIVECOIN_CORE_SERIALIZE_H

#include <string>
#include <list>
#include <functional>

#include <naivecoin/core/block.h>

namespace naivecoin::core {

Block deserialize_block(std::string const & text);
std::list<Block> deserialize_blockchain(std::string const & text);
std::string serialize_blockchain(std::list<Block> const & blockchain);

std::string create_send_block_message(Block const & block, std::string const & sender);
std::string create_send_blockchain_message(std::list<Block> const & blockchain, std::string const & sender);
std::string create_query_latest_block_message(std::string const & sender);
std::string create_query_blockchain_message(std::string const & sender);

void process_message(
    std::string const & serialized_message,
    std::function<void(Block const &, std::string const &)> const & process_send_block_message,
    std::function<void(std::list<Block> const &, std::string const &)> const & process_send_blockchain_message,
    std::function<void(std::string const &)> const & process_query_latest_block_message,
    std::function<void(std::string const &)> const & process_query_blockchain_message,
    std::function<void(std::string const &, std::string const &)> const & process_unknown_message,
    std::function<void(std::string const &)> const & process_invalid_message
);

} // namespace naivecoin::core

#endif // NAIVECOIN_CORE_SERIALIZE_H
