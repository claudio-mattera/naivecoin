#include <boost/test/unit_test.hpp>

#include <naivecoin/transaction/output.h>

BOOST_AUTO_TEST_SUITE(TransactionOutput)

BOOST_AUTO_TEST_CASE(constructor)
{
    std::string const address = "";
    uint64_t const amount = 12;

    naivecoin::transaction::Output output(address, amount);

    BOOST_CHECK_EQUAL(output.address, address);
    BOOST_CHECK_EQUAL(output.amount, amount);
}

BOOST_AUTO_TEST_SUITE_END()
