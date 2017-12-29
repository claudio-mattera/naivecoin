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
        throw new std::invalid_argument("Hash mismatch");
    }

    return block;
}

} // unnamed namespace

namespace naivecoin {

std::string serialize_block(Block const & block)
{
    Json::Value const root = serialize_block_to_json(block);
    return format_json(root);
}

std::string serialize_blockchain(std::list<Block> const & blockchain)
{
    Json::Value const array = serialize_blockchain_to_json(blockchain);
    return format_json(array);
}

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
        throw new std::invalid_argument("Hash mismatch");
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

} // namespace naivecoin
