#include <naivecoin/core/serialize.h>

#include <stdexcept>
#include <sstream>

#include <jsoncpp/json/json.h>

#include <naivecoin/core/utils.h>

namespace {

using namespace naivecoin::core;

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

Json::Value serialize_block_to_json(Block const & block)
{
    Json::Value value;

    value["index"] = block.index;
    value["hash"] = block.hash;
    value["previous_hash"] = block.previous_hash;
    value["timestamp"] = format_timestamp(block.timestamp);
    value["data"] = block.data;
    value["difficulty"] = block.difficulty;
    value["nonce"] = block.nonce;

    return value;
}

template<template<class> class Iterator>
Json::Value serialize_blockchain_to_json(Iterator<Block> const begin, Iterator<Block> const end)
{
    Json::Value array(Json::arrayValue);

    for (Iterator i = begin; i != end; ++i) {
        array.append(serialize_block_to_json(*i));
    }

    return array;
}

Block deserialize_json_to_block(Json::Value const & value)
{
    auto timestamp = parse_timestamp(value["timestamp"].asString());

    Block const block = Block::make_block(
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

template<class Container>
void deserialize_json_to_blockchain(std::insert_iterator<Container> dest, Json::Value const & array)
{
    for (auto value: array) {
        *dest++ = deserialize_json_to_block(value);
    }
}

Json::Value create_message(
    std::string const & message,
    Json::Value const & data,
    std::string const & sender
)
{
    Json::Value value;

    value["sender"] = sender;
    value["message"] = message;
    value["data"] = data;

    return value;
}

} // unnamed namespace

namespace naivecoin::core {

Block deserialize_block(std::string const & text)
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
std::string serialize_blockchain(Iterator<Block> const begin, Iterator<Block> const end)
{
    Json::Value const value = serialize_blockchain_to_json(begin, end);
    return format_json(value);
}

std::string create_send_block_message(Block const & block, std::string const & sender)
{
    Json::Value const data = serialize_block_to_json(block);
    Json::Value const message = create_message("send block", data, sender);
    return format_json(message);
}

template<template<class> class Iterator>
std::string create_send_blockchain_message(Iterator<Block> const begin, Iterator<Block> const end, std::string const & sender)
{
    Json::Value const data = serialize_blockchain_to_json(begin, end);
    Json::Value const message = create_message("send blockchain", data, sender);
    return format_json(message);
}

std::string create_query_latest_block_message(std::string const & sender)
{
    Json::Value const message = create_message("query latest block", Json::Value(), sender);
    return format_json(message);
}

std::string create_query_blockchain_message(std::string const & sender)
{
    Json::Value const message = create_message("query blockchain", Json::Value(), sender);
    return format_json(message);
}

void process_message(
    std::string const & serialized_message,
    std::function<void(Block const &, std::string const &)> const & process_send_block_message,
    std::function<void(std::list<Block> const &, std::string const &)> const & process_send_blockchain_message,
    std::function<void(std::string const &)> const & process_query_latest_block_message,
    std::function<void(std::string const &)> const & process_query_blockchain_message,
    std::function<void(std::string const &, std::string const &)> const & process_unknown_message,
    std::function<void(std::string const &)> const & process_invalid_message
)
{
    try {
        Json::Value const root = parse_json(serialized_message);
        if (! root.isObject()) {
            throw Json::RuntimeError("Not a JSON object");
        }
        std::string const sender = root["sender"].asString();
        std::string const message = root["message"].asString();

        if (message == "send block") {
            Block const block = deserialize_json_to_block(root["data"]);
            process_send_block_message(block, sender);
        } else if (message == "send blockchain") {
            std::list<Block> blockchain;
            deserialize_json_to_blockchain(
                std::insert_iterator<std::list<Block>>(blockchain, std::begin(blockchain)),
                root["data"]
            );
            process_send_blockchain_message(blockchain, sender);
        } else if (message == "query latest block") {
            process_query_latest_block_message(sender);
        } else if (message == "query blockchain") {
            process_query_blockchain_message(sender);
        } else {
            process_unknown_message(message, sender);
        }
    } catch (Json::RuntimeError const & exception) {
        process_invalid_message(exception.what());
    } catch (std::exception const & exception) {
        process_invalid_message(exception.what());
    }
}


// Explicit template instantiations

template
std::string serialize_blockchain<>(
    std::list<Block>::const_iterator const begin,
    std::list<Block>::const_iterator const end
);

template
std::string create_send_blockchain_message<>(
    std::list<Block>::const_iterator const begin,
    std::list<Block>::const_iterator const end,
    std::string const & sender
);

template
void deserialize_blockchain(
    std::insert_iterator<std::list<Block>> dest,
    std::string const & text
);

} // namespace naivecoin::core
