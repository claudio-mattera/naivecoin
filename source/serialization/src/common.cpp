#include "common.h"

#include <sstream>
#include <list>

#include <naivecoin/time/time.h>

namespace naivecoin::serialization {

using namespace naivecoin;

std::string format_json(Json::Value const & value)
{
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

Json::Value parse_json(std::string const & text)
{
    // Do not use std::ostringstream, JsonCpp expect a full std::stringstream
    std::stringstream stream(text);
    Json::Value value;
    stream >> value;
    return value;
}

Json::Value serialize_block_to_json(core::Block const & block)
{
    Json::Value value;

    value["index"] = Json::Value::UInt64(block.index);
    value["hash"] = block.hash;
    value["previous_hash"] = block.previous_hash;
    value["timestamp"] = time::format_timestamp(block.timestamp);
    value["data"] = block.data;
    value["difficulty"] = Json::Value::UInt(block.difficulty);
    value["nonce"] = Json::Value::UInt64(block.nonce);

    return value;
}

core::Block deserialize_json_to_block(Json::Value const & value)
{
    auto timestamp = time::parse_timestamp(value["timestamp"].asString());

    core::Block const block = core::Block::make_block(
        static_cast<uint16_t>(value["index"].asUInt64()),
        value["previous_hash"].asString(),
        timestamp,
        value["data"].asString(),
        value["difficulty"].asUInt(),
        value["nonce"].asUInt64()
    );

    if (block.hash != value["hash"].asString()) {
        std::ostringstream stream;
        stream << "Hash mismatch " << block.hash << " != " << value["hash"].asString();
        throw std::invalid_argument(stream.str());
    }

    return block;
}

template<template<class> class Iterator>
Json::Value serialize_blockchain_to_json(Iterator<core::Block> const begin, Iterator<core::Block> const end)
{
    Json::Value array(Json::arrayValue);

    for (Iterator i = begin; i != end; ++i) {
        array.append(serialize_block_to_json(*i));
    }

    return array;
}

template<class Container>
void deserialize_json_to_blockchain(std::insert_iterator<Container> dest, Json::Value const & array)
{
    for (auto value: array) {
        *dest++ = deserialize_json_to_block(value);
    }
}


// Explicit template instantiations

template
Json::Value serialize_blockchain_to_json<>(
    std::list<core::Block>::const_iterator const begin,
    std::list<core::Block>::const_iterator const end
);

template
void deserialize_json_to_blockchain<>(
    std::insert_iterator<std::list<core::Block>> dest,
    Json::Value const & array
);

} // namespace naivecoin::serialization
