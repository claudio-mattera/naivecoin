#include <naivecoin/transaction/input.h>

namespace naivecoin {

Input::Input(
    std::string const & transaction_output_id,
    uint16_t const transaction_output_index,
    std::string const & signature
)
: transaction_output_id(transaction_output_id)
, transaction_output_index(transaction_output_index)
, signature(signature)
{
}

std::ostream & operator<<(std::ostream & stream, Input const & input)
{
    stream
        << "TxOutId: " << input.transaction_output_id << ", "
        << "TxOutIndex: " << input.transaction_output_index << ", "
        << "Signature: " << input.signature;
    return stream;
}

} // namespace naivecoin
