#include <naivecoin/core/serialize.h>

#include <stdexcept>
#include <sstream>

#include <jsoncpp/json/json.h>

#include <naivecoin/core/utils.h>

namespace {

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

Json::Value serialize_block_to_json(naivecoin::Block const & block)
{
    Json::Value value;

    value["index"] = block.index;
    value["hash"] = block.hash;
    value["previous_hash"] = block.previous_hash;
    value["timestamp"] = naivecoin::format_timestamp(block.timestamp);
    value["data"] = block.data;
    value["difficulty"] = block.difficulty;
    value["nonce"] = block.nonce;

    return value;
}

Json::Value serialize_blockchain_to_json(std::list<naivecoin::Block> const & blockchain)
{
    Json::Value array(Json::arrayValue);

    for (auto block: blockchain) {
        array.append(serialize_block_to_json(block));
    }

    return array;
}

naivecoin::Block deserialize_json_to_block(Json::Value const & value)
{
    auto timestamp = naivecoin::parse_timestamp(value["timestamp"].asString());

    naivecoin::Block block = naivecoin::Block::make_block(
        static_cast<uint16_t>(value["index"].asInt()),
        value["previous_hash"].asString(),
        timestamp,
        value["data"].asString(),
        value["difficulty"].asInt(),
        value["nonce"].asInt()
    );

    if (block.hash != value["hash"].asString()) {
        std::ostringstream stream;
        stream << "Hash mismatch " << block.hash << " != " << value["hash"].asString();
        throw std::invalid_argument(stream.str());
    }

    return block;
}

std::list<naivecoin::Block> deserialize_json_to_blockchain(Json::Value const & array)
{
    std::list<naivecoin::Block> blockchain;

    for (auto value: array) {
        blockchain.push_back(deserialize_json_to_block(value));
    }

    return blockchain;
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

namespace naivecoin {

Block deserialize_block(std::string const & text)
{
    Json::Value const value = parse_json(text);
    auto timestamp = naivecoin::parse_timestamp(value["timestamp"].asString());

    naivecoin::Block block = naivecoin::Block::make_block(
        static_cast<uint16_t>(value["index"].asInt()),
        value["previous_hash"].asString(),
        timestamp,
        value["data"].asString(),
        value["difficulty"].asInt(),
        value["nonce"].asInt()
    );

    if (block.hash != value["hash"].asString()) {
        throw std::invalid_argument("Hash mismatch");
    }

    return block;
}

std::list<Block> deserialize_blockchain(std::string const & text)
{
    Json::Value const array = parse_json(text);
    std::list<naivecoin::Block> blockchain;

    for (auto value: array) {
        blockchain.push_back(deserialize_json_to_block(value));
    }

    return blockchain;
}

std::string serialize_blockchain(std::list<Block> const & blockchain)
{
    Json::Value const value = serialize_blockchain_to_json(blockchain);
    return format_json(value);
}

std::string create_send_block_message(Block const & block, std::string const & sender)
{
    Json::Value const data = serialize_block_to_json(block);
    Json::Value const message = create_message("send block", data, sender);
    return format_json(message);
}

std::string create_send_blockchain_message(std::list<Block> const & blockchain, std::string const & sender)
{
    Json::Value const data = serialize_blockchain_to_json(blockchain);
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
            std::list<Block> const blockchain = deserialize_json_to_blockchain(root["data"]);
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

} // namespace naivecoin
