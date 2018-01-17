#include <naivecoin/transaction/transaction.h>

#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <numeric>

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

bool is_transaction_input_valid(
    naivecoin::Input const & input,
    naivecoin::Transaction const & transaction,
    std::list<naivecoin::UnspentOutput> const & unspent_outputs
)
{
    auto const referenced_unspent_output_iterator = std::find_if(
        std::begin(unspent_outputs),
        std::end(unspent_outputs),
        [&input](auto const & unspent_output) {
            return unspent_output.transaction_id == input.transaction_output_id;
        }
    );

    if (referenced_unspent_output_iterator == std::end(unspent_outputs)) {
        return false;
    }

    std::string const & address = referenced_unspent_output_iterator->address;

    bool const verified = naivecoin::verify(transaction.id, input.signature, address);

    return verified;
}

} // unnamed namespace

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

bool is_transaction_list_valid(
    uint64_t const index,
    std::list<Transaction> const & transactions,
    std::list<UnspentOutput> const & unspent_outputs
)
{
    if (transactions.empty()) {
        // There must be at least a coinbase transaction.
        return false;
    }

    auto iterator = std::begin(transactions);

    Transaction coinbase_transaction = * iterator;
    ++iterator;

    if (! is_coinbase_transaction_valid(index, coinbase_transaction))

    while (iterator != std::end(transactions)) {
        if (! is_transaction_valid(* iterator, unspent_outputs)) {
            return false;
        }
        ++iterator;
    }

    return true;
}

bool is_coinbase_transaction_valid(
    uint64_t const index,
    Transaction const & coinbase_transaction
)
{
    if (compute_transaction_id(coinbase_transaction) != coinbase_transaction.id) {
        return false;
    }
    if (coinbase_transaction.inputs.size() != 1) {
        // Coinbase transaction has exactly one input.
        return false;
    }
    if (coinbase_transaction.inputs.front().transaction_output_index != index) {
        // Coinbase transaction output refers to the current block index.
        return false;
    }
    if (coinbase_transaction.outputs.size() != 1) {
        // Coinbase transaction has exactly one output.
        return false;
    }
    if (coinbase_transaction.outputs.front().amount != naivecoin::COINBASE_AMOUNT) {
        return false;
    }

    return true;
}

bool is_transaction_valid(
    Transaction const & transaction,
    std::list<UnspentOutput> const & unspent_outputs
)
{
    if (compute_transaction_id(transaction) != transaction.id) {
        return false;
    }

    bool const are_inputs_valid = std::all_of(
        std::begin(transaction.inputs),
        std::end(transaction.inputs),
        [&transaction, &unspent_outputs](auto input) {
            return is_transaction_input_valid(input, transaction, unspent_outputs);
        }
    );
    if (! are_inputs_valid) {
        return false;
    }

    uint64_t const total_from_input = std::accumulate(
        std::begin(transaction.inputs),
        std::end(transaction.inputs),
        0,
        [&unspent_outputs](auto accumulator, auto input) {

            auto const referenced_unspent_output_iterator = std::find_if(
                std::begin(unspent_outputs),
                std::end(unspent_outputs),
                [&input, &unspent_outputs](auto const & unspent_output) {
                    return unspent_output.transaction_id == input.transaction_output_id;
                }
            );

            if (referenced_unspent_output_iterator == std::end(unspent_outputs)) {
                throw std::invalid_argument("Non-existent unspent output");
            }

            return accumulator + referenced_unspent_output_iterator->amount;
        }
    );

    uint64_t const total_from_output = std::accumulate(
        std::begin(transaction.outputs),
        std::end(transaction.outputs),
        0,
        [](auto accumulator, auto output) {
            return accumulator +  output.amount;
        }
    );

    if (total_from_input != total_from_output) {
        return false;
    }

    return true;
}

std::string compute_input_signature(
    std::string const & transaction_id,
    Input const & input,
    std::string const & private_key,
    std::list<UnspentOutput> const & unspent_outputs
)
{
    std::string const data_to_sign = transaction_id;

    UnspentOutput const referenced_unspent_output = find_unspent_output(
        input.transaction_output_id,
        input.transaction_output_index,
        unspent_outputs
    );
    std::string const referenced_address = referenced_unspent_output.address;

    std::string const signature = sign(data_to_sign, private_key);
    return signature;
}

Transaction create_transaction(
    std::list<Input> inputs,
    std::list<Output> const & outputs,
    std::list<std::string> const & private_keys,
    std::list<UnspentOutput> const & unspent_outputs
)
{
    std::string const transaction_id = ::compute_transaction_id(inputs, outputs);

    if (private_keys.empty()) {
        // Coinbase transaction, not signing the input
    } else {
        auto input_iterator = std::begin(inputs);
        auto private_key_iterator = std::begin(private_keys);
        while (input_iterator != std::end(inputs)) {
            input_iterator->signature = compute_input_signature(
                transaction_id,
                * input_iterator,
                * private_key_iterator,
                unspent_outputs
            );
            ++input_iterator;
            ++private_key_iterator;
        }
    }

    return Transaction(transaction_id, inputs, outputs);
}

Transaction create_coinbase_transaction(uint64_t const index, std::string const & address)
{
    return create_transaction(
        {Input("", index)},
        {Output(address, naivecoin::COINBASE_AMOUNT)},
        {},
        {}
    );
}

} // namespace naivecoin
