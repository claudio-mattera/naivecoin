#include <naivecoin/serialization/transaction.h>

#include "common.h"

#include <naivecoin/transaction/input.h>
#include <naivecoin/transaction/output.h>

#include <sstream>
#include <stdexcept>

namespace {

using namespace naivecoin;

Json::Value serialize_output_to_json(transaction::Output const & output)
{
    Json::Value value;
    value["address"] = output.address;
    value["amount"] = output.amount;
    return value;
}

Json::Value serialize_outputs_to_json(std::list<transaction::Output> const & outputs)
{
    Json::Value array(Json::arrayValue);
    for (auto output: outputs) {
        array.append(serialize_output_to_json(output));
    }
    return array;
}

Json::Value serialize_input_to_json(transaction::Input const & input)
{
    Json::Value value;
    value["transaction_output_id"] = input.transaction_output_id;
    value["transaction_output_index"] = input.transaction_output_index;
    value["signature"] = input.signature;
    return value;
}

Json::Value serialize_inputs_to_json(std::list<transaction::Input> const & inputs)
{
    Json::Value array(Json::arrayValue);
    for (auto input: inputs) {
        array.append(serialize_input_to_json(input));
    }
    return array;
}

Json::Value serialize_transaction_to_json(transaction::Transaction const & transaction)
{
    Json::Value value;
    value["id"] = transaction.id;
    value["inputs"] = serialize_inputs_to_json(transaction.inputs);
    value["outputs"] = serialize_outputs_to_json(transaction.outputs);
    return value;
}

Json::Value serialize_transactions_to_json(std::list<transaction::Transaction> const & transactions)
{
    Json::Value array(Json::arrayValue);
    for (auto transaction: transactions) {
        array.append(serialize_transaction_to_json(transaction));
    }
    return array;
}

transaction::Output deserialize_json_to_output(Json::Value const & value)
{
    transaction::Output const output(
        value["address"].asString(),
        value["amount"].asUInt64()
    );
    return output;
}

std::list<transaction::Output> deserialize_json_to_outputs(Json::Value const & array)
{
    std::list<transaction::Output> outputs;
    for (Json::Value value: array) {
        outputs.push_back(deserialize_json_to_output(value));
    }
    return outputs;
}

transaction::Input deserialize_json_to_input(Json::Value const & value)
{
    transaction::Input const input(
        value["transaction_output_id"].asString(),
        value["transaction_output_index"].asUInt(),
        value["signature"].asString()
    );
    return input;
}

std::list<transaction::Input> deserialize_json_to_inputs(Json::Value const & array)
{
    std::list<transaction::Input> inputs;
    for (Json::Value value: array) {
        inputs.push_back(deserialize_json_to_input(value));
    }
    return inputs;
}

transaction::Transaction deserialize_json_to_transaction(Json::Value const & value)
{
    transaction::Transaction const transaction(
        value["id"].asString(),
        deserialize_json_to_inputs(value["inputs"]),
        deserialize_json_to_outputs(value["outputs"])
    );
    return transaction;
}

std::list<transaction::Transaction> deserialize_json_to_transactions(Json::Value const & array)
{
    std::list<transaction::Transaction> transactions;
    for (Json::Value value: array) {
        transactions.push_back(deserialize_json_to_transaction(value));
    }
    return transactions;
}

} // unnamed namespace

namespace naivecoin::serialization {

std::string serialize_transactions(std::list<transaction::Transaction> const & transactions)
{
    Json::Value const value = serialize_transactions_to_json(transactions);
    return format_json(value);
}

std::list<transaction::Transaction> deserialize_transactions(std::string const & text)
{
    Json::Value const value = parse_json(text);
    return deserialize_json_to_transactions(value);
}

} // namespace naivecoin::serialization
