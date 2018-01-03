#include <boost/test/unit_test.hpp>

#include <naivecoin/transaction/unspent_output.h>

BOOST_AUTO_TEST_SUITE(TransactionUnspentOutput)

BOOST_AUTO_TEST_CASE(constructor)
{
    std::string const transaction_id = "id";
    uint64_t const transaction_index = 45;
    std::string const address = "";
    uint64_t const amount = 12;

    naivecoin::UnspentOutput unspent_output(transaction_id, transaction_index, address, amount);

    BOOST_CHECK_EQUAL(unspent_output.transaction_id, transaction_id);
    BOOST_CHECK_EQUAL(unspent_output.transaction_index, transaction_index);
    BOOST_CHECK_EQUAL(unspent_output.address, address);
    BOOST_CHECK_EQUAL(unspent_output.amount, amount);
}

BOOST_AUTO_TEST_SUITE_END()
