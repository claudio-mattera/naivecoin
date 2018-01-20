#include <naivecoin/serialization/message.h>

#include <list>

#include "common.h"

namespace {

using namespace naivecoin::core;

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

namespace naivecoin::serialization {

using namespace naivecoin;

std::string create_send_block_message(core::Block const & block, std::string const & sender)
{
    Json::Value const data = serialize_block_to_json(block);
    Json::Value const message = create_message("send block", data, sender);
    return format_json(message);
}

template<template<class> class Iterator>
std::string create_send_blockchain_message(Iterator<core::Block> const begin, Iterator<core::Block> const end, std::string const & sender)
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
    std::function<void(core::Block const &, std::string const &)> const & process_send_block_message,
    std::function<void(std::list<core::Block> const &, std::string const &)> const & process_send_blockchain_message,
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
            core::Block const block = deserialize_json_to_block(root["data"]);
            process_send_block_message(block, sender);
        } else if (message == "send blockchain") {
            std::list<core::Block> blockchain;
            deserialize_json_to_blockchain(
                std::insert_iterator<std::list<core::Block>>(blockchain, std::begin(blockchain)),
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
std::string create_send_blockchain_message<>(
    std::list<core::Block>::const_iterator const begin,
    std::list<core::Block>::const_iterator const end,
    std::string const & sender
);

} // namespace naivecoin::serialization
