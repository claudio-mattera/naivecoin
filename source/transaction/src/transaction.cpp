#include <naivecoin/transaction/transaction.h>

#include <iomanip>

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

} // namespace naivecoin
