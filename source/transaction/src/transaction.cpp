#include <naivecoin/transaction/transaction.h>

#include <iomanip>

#include <naivecoin/transaction/utils.h>

#include <naivecoin/crypto/crypto.h>

namespace naivecoin {

Transaction::Transaction(
    std::string const & id,
    std::list<Input> const & inputs,
    std::list<Output> const & outputs
)
: id(id)
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
    std::string const inputs_string = join_inputs(transaction.inputs);
    std::string const outputs_string = join_outputs(transaction.outputs);

    std::string const whole_string = inputs_string + outputs_string;

    return compute_hash(whole_string);
}

} // namespace naivecoin
