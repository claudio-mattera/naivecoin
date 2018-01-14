#include <naivecoin/transaction/transaction.h>

#include <iomanip>
#include <stdexcept>
#include <algorithm>

#include <naivecoin/transaction/utils.h>

#include <naivecoin/crypto/crypto.h>

namespace {

std::string compute_transaction_id(
    std::list<naivecoin::Input> const & inputs,
    std::list<naivecoin::Output> const & outputs
)
{
    std::string const inputs_string = join_inputs(inputs);
    std::string const outputs_string = join_outputs(outputs);

    std::string const whole_string = inputs_string + outputs_string;

    return naivecoin::compute_hash(whole_string);
}

naivecoin::UnspentOutput find_unspent_output(
    std::string const & transaction_output_id,
    uint16_t const transaction_output_index,
    std::list<naivecoin::UnspentOutput> const & unspent_outputs
)
{
    for (auto unspent_output: unspent_outputs) {
        if (transaction_output_id == unspent_output.transaction_id
            && transaction_output_index == unspent_output.transaction_index) {
            return unspent_output;
        }
    }

    throw std::invalid_argument("Cannot find unspent output");
}

} // unnamed namespace

namespace naivecoin {

Transaction::Transaction(
    std::list<Input> const & inputs,
    std::list<Output> const & outputs
)
: id(::compute_transaction_id(inputs, outputs))
, inputs(inputs)
, outputs(outputs)
{
}

std::ostream & operator<<(std::ostream & stream, Transaction const & transaction)
{
    stream << "Id: " << transaction.id << ", Inputs: {";
    for (auto input: transaction.inputs) {
        stream << input << ", ";
    }
    stream << "}, Outputs: {";
    for (auto output: transaction.outputs) {
        stream << output << ", ";
    }
    stream << "}";

    return stream;
}

std::string compute_transaction_id(Transaction const & transaction)
{
    return ::compute_transaction_id(transaction.inputs, transaction.outputs);
}

std::string compute_input_signature(
    Transaction const & transaction,
    uint64_t const transaction_input_index,
    std::string const & private_key,
    std::list<UnspentOutput> const & unspent_outputs
)
{
    auto iterator = std::begin(transaction.inputs);
    std::advance(iterator, transaction_input_index);
    Input const & input = * iterator;

    std::string const data_to_sign = transaction.id;

    UnspentOutput const referenced_unspent_output = find_unspent_output(
        input.transaction_output_id,
        input.transaction_output_index,
        unspent_outputs
    );
    std::string const referenced_address = referenced_unspent_output.address;

    std::string const signature = sign(data_to_sign, private_key);
    return signature;
}

} // namespace naivecoin
