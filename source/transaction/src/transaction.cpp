#include <naivecoin/transaction/transaction.h>

#include <iomanip>

#include <openssl/sha.h>

#include <naivecoin/transaction/utils.h>

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

    unsigned char hash[SHA_DIGEST_LENGTH]; // == 20
    SHA1(reinterpret_cast<const unsigned char*>(whole_string.c_str()), whole_string.size(), hash);

    std::ostringstream stream;

    for(int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        stream
            << std::setfill('0')
            << std::setw(2)
            << std::hex
            << static_cast<unsigned int>(hash[i]);
    }

    return stream.str();
}

} // namespace naivecoin
