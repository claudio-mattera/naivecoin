#include <naivecoin/serialization/unspent_output.h>

#include "common.h"

#include <sstream>
#include <stdexcept>

namespace {

using namespace naivecoin;

Json::Value serialize_unspent_output_to_json(transaction::UnspentOutput const & unspent_output)
{
    Json::Value value;
    value["transaction_id"] = unspent_output.transaction_id;
    value["transaction_index"] = Json::Value::UInt64(unspent_output.transaction_index);
    value["address"] = unspent_output.address;
    value["amount"] = Json::Value::UInt64(unspent_output.amount);
    return value;
}

Json::Value serialize_unspent_outputs_to_json(std::list<transaction::UnspentOutput> const & unspent_outputs)
{
    Json::Value array(Json::arrayValue);
    for (auto unspent_output: unspent_outputs) {
        array.append(serialize_unspent_output_to_json(unspent_output));
    }
    return array;
}

transaction::UnspentOutput deserialize_json_to_unspent_output(Json::Value const & value)
{
    transaction::UnspentOutput const unspent_output(
        value["transaction_id"].asString(),
        value["transaction_index"].asUInt64(),
        value["address"].asString(),
        value["amount"].asUInt64()
    );
    return unspent_output;
}

std::list<transaction::UnspentOutput> deserialize_json_to_unspent_outputs(Json::Value const & array)
{
    std::list<transaction::UnspentOutput> unspent_outputs;
    for (Json::Value value: array) {
        unspent_outputs.push_back(deserialize_json_to_unspent_output(value));
    }
    return unspent_outputs;
}

} // unnamed namespace

namespace naivecoin::serialization {

std::string serialize_unspent_outputs(std::list<naivecoin::transaction::UnspentOutput> const & unspent_outputs)
{
    Json::Value const value = serialize_unspent_outputs_to_json(unspent_outputs);
    return format_json(value);
}

std::list<naivecoin::transaction::UnspentOutput> deserialize_unspent_outputs(std::string const & text)
{
    Json::Value const value = parse_json(text);
    return deserialize_json_to_unspent_outputs(value);
}

} // namespace naivecoin::serialization
