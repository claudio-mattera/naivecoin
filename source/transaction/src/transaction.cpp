#include <naivecoin/transaction/transaction.h>

namespace naivecoin {

Transaction::Transaction(
    std::string const & id,
    std::list<Input> const & input,
    std::list<Output> const & output
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

} // namespace naivecoin
