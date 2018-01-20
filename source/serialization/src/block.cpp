#include <naivecoin/serialization/block.h>

#include "common.h"

#include <list>
#include <sstream>

namespace {

using namespace naivecoin;

} // unnamed namespace

namespace naivecoin::serialization {

using namespace naivecoin;

core::Block deserialize_block(std::string const & text)
{
    Json::Value const value = parse_json(text);
    return deserialize_json_to_block(value);
}

template<class Container>
void deserialize_blockchain(std::insert_iterator<Container> dest, std::string const & text)
{
    Json::Value const array = parse_json(text);
    deserialize_json_to_blockchain(dest, array);
}

template<template<class> class Iterator>
std::string serialize_blockchain(Iterator<core::Block> const begin, Iterator<core::Block> const end)
{
    Json::Value const value = serialize_blockchain_to_json(begin, end);
    return format_json(value);
}


// Explicit template instantiations

template
std::string serialize_blockchain<>(
    std::list<core::Block>::const_iterator const begin,
    std::list<core::Block>::const_iterator const end
);

template
void deserialize_blockchain(
    std::insert_iterator<std::list<core::Block>> dest,
    std::string const & text
);

} // namespace naivecoin::serialization
