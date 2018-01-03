#include <boost/test/unit_test.hpp>

#include <naivecoin/transaction/transaction.h>

BOOST_AUTO_TEST_SUITE(TransactionTransaction)

BOOST_AUTO_TEST_CASE(constructor)
{
    std::string const id = "";
    std::list<naivecoin::Input> inputs;
    std::list<naivecoin::Output> outputs;

    naivecoin::Transaction transaction(id, inputs, outputs);

    BOOST_CHECK_EQUAL(transaction.id, id);
    //BOOST_CHECK(transaction.inputs == inputs);
    //BOOST_CHECK(transaction.outputs == outputs);
}

BOOST_AUTO_TEST_SUITE_END()
