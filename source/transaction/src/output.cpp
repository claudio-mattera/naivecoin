#include <naivecoin/transaction/output.h>

namespace naivecoin {

Output::Output(
    std::string const address,
    uint64_t const amount
    )
: address(address)
, amount(amount)
{
}

std::ostream & operator<<(std::ostream & stream, Output const & output)
{
    stream
        << "Address: " << output.address << ", "
        << "Amount: " << output.amount;
    return stream;
}

} // namespace naivecoin
