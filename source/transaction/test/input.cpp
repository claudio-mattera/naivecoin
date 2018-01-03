#include <boost/test/unit_test.hpp>

#include <naivecoin/transaction/input.h>

BOOST_AUTO_TEST_SUITE(TransactionInput)

BOOST_AUTO_TEST_CASE(constructor)
{
    std::string const transaction_output_id = "";
    uint64_t const transaction_output_index = 12;
    std::string const signature = "";

    naivecoin::Input input(transaction_output_id, transaction_output_index, signature);

    BOOST_CHECK_EQUAL(input.transaction_output_id, transaction_output_id);
    BOOST_CHECK_EQUAL(input.transaction_output_index, transaction_output_index);
    BOOST_CHECK_EQUAL(input.signature, signature);
}

BOOST_AUTO_TEST_SUITE_END()
