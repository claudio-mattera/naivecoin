
#pragma once

#ifndef NAIVECOIN_SERIALIZATION_COMMON_H
#define NAIVECOIN_SERIALIZATION_COMMON_H

#include <string>

#include <jsoncpp/json/json.h>

#include <naivecoin/core/block.h>
#include <naivecoin/transaction/input.h>
#include <naivecoin/transaction/output.h>

namespace naivecoin::serialization {

std::string format_json(Json::Value const & value);
Json::Value parse_json(std::string const & text);


Json::Value serialize_block_to_json(naivecoin::core::Block const & block);

naivecoin::core::Block deserialize_json_to_block(Json::Value const & value);

template<template<class> class Iterator>
Json::Value serialize_blockchain_to_json(Iterator<naivecoin::core::Block> const begin, Iterator<naivecoin::core::Block> const end);

template<class Container>
void deserialize_json_to_blockchain(std::insert_iterator<Container> dest, Json::Value const & array);

} // namespace naivecoin::serialization

#endif // NAIVECOIN_SERIALIZATION_COMMON_H
