#include <boost/test/unit_test.hpp>

#include <list>

#include <naivecoin/transaction/transaction.h>

BOOST_AUTO_TEST_SUITE(TransactionTransaction)

BOOST_AUTO_TEST_CASE(constructor)
{
    std::list<naivecoin::Input> inputs;
    std::list<naivecoin::Output> outputs;

    naivecoin::Transaction transaction(inputs, outputs);

    //BOOST_CHECK(transaction.inputs == inputs);
    //BOOST_CHECK(transaction.outputs == outputs);
}

BOOST_AUTO_TEST_CASE(compute_transaction_id)
{
    std::list<naivecoin::Input> const inputs{
        naivecoin::Input("some id", 23, ""),
        naivecoin::Input("some other id", 54, ""),
        naivecoin::Input("a third id", 458, "")
    };
    std::list<naivecoin::Output> const outputs{
        naivecoin::Output("12345", 7564),
        naivecoin::Output("98765", 546),
        naivecoin::Output("0001", 5436)
    };

    naivecoin::Transaction transaction(inputs, outputs);

    std::string const actual = naivecoin::compute_transaction_id(transaction);

    std::string const expected = "f515a03fd85da81a23ea16b227678fdd7ec99de8";

    BOOST_CHECK_EQUAL(actual, expected);
}

BOOST_AUTO_TEST_SUITE_END()
