#include <naivecoin/transaction/unspent_output.h>

namespace naivecoin::transaction {

UnspentOutput::UnspentOutput(
    std::string const & transaction_id,
    uint64_t const transaction_index,
    std::string const & address,
    uint64_t const amount
)
: transaction_id(transaction_id)
, transaction_index(transaction_index)
, address(address)
, amount(amount)
{
}

std::ostream & operator<<(std::ostream & stream, UnspentOutput const & unspent_output)
{
    stream
        << "Id: " << unspent_output.transaction_id << ", "
        << "Index: " << unspent_output.transaction_index << ", "
        << "Address: " << unspent_output.address << ", "
        << "Amount: " << unspent_output.amount;
    return stream;
}

} // namespace naivecoin::transaction
