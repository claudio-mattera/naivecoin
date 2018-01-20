
#pragma once

#ifndef NAIVECOIN_SERIALIZATION_MESSAGE_H
#define NAIVECOIN_SERIALIZATION_MESSAGE_H

#include <functional>
#include <list>

#include <naivecoin/core/block.h>

namespace naivecoin::serialization {

std::string create_send_block_message(naivecoin::core::Block const & block, std::string const & sender);

template<template<class> class Iterator>
std::string create_send_blockchain_message(Iterator<naivecoin::core::Block> const begin, Iterator<naivecoin::core::Block> const end, std::string const & sender);
std::string create_query_latest_block_message(std::string const & sender);
std::string create_query_blockchain_message(std::string const & sender);

void process_message(
    std::string const & serialized_message,
    std::function<void(naivecoin::core::Block const &, std::string const &)> const & process_send_block_message,
    std::function<void(std::list<naivecoin::core::Block> const &, std::string const &)> const & process_send_blockchain_message,
    std::function<void(std::string const &)> const & process_query_latest_block_message,
    std::function<void(std::string const &)> const & process_query_blockchain_message,
    std::function<void(std::string const &, std::string const &)> const & process_unknown_message,
    std::function<void(std::string const &)> const & process_invalid_message
);

} // namespace naivecoin::serialization

#endif // NAIVECOIN_SERIALIZATION_MESSAGE_H
