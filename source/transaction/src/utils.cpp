#include <naivecoin/transaction/utils.h>

#include <sstream>
#include <iterator>
#include <algorithm>

namespace naivecoin::transaction {

std::string join_inputs(std::list<Input> const & inputs)
{
    std::ostringstream stream;

    for (auto input: inputs) {
        stream
            << "I{" << input.transaction_output_id << ","
            << input.transaction_output_index << "}";
    }

    return stream.str();
}

std::string join_outputs(std::list<Output> const & outputs)
{
    std::ostringstream stream;

    for (auto output: outputs) {
        stream
            << "O{" << output.address << ","
            << output.amount << "}";
    }

    return stream.str();
}

} // namespace naivecoin::transaction
