#include <naivecoin/core/serialize.h>

#include <stdexcept>
#include <sstream>

#include <jsoncpp/json/json.h>

#include <naivecoin/core/utils.h>

namespace naivecoin {

std::string serialize_block(Block const & block)
{
    Json::Value root;

    root["index"] = block.index;
    root["hash"] = block.hash;
    root["previous_hash"] = block.previous_hash;
    root["timestamp"] = naivecoin::format_timestamp(block.timestamp);
    root["data"] = block.data;

    std::ostringstream stream;
    stream << root;

    return stream.str();
}

std::string serialize_blockchain(std::list<Block> const & blockchain)
{
}

Block deserialize_block(std::string const & text)
{
    // Do not use std::ostringstream, JsonCpp expect a full std::stringstream
    std::stringstream stream(text);
    Json::Value root;
    stream >> root;

    auto timestamp = naivecoin::parse_timestamp(root["timestamp"].asString());

    naivecoin::Block block = naivecoin::Block::make_block(
        static_cast<uint16_t>(root["index"].asInt()),
        root["previous_hash"].asString(),
        timestamp,
        root["data"].asString()
    );

    if (block.hash != root["hash"].asString()) {
        throw new std::invalid_argument("Hash mismatch");
    }

    return block;
}

std::list<Block> deserialize_blockchain(std::string const & text)
{
}

} // namespace naivecoin
