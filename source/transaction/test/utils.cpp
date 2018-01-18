#include <boost/test/unit_test.hpp>

#include <naivecoin/transaction/utils.h>

BOOST_AUTO_TEST_SUITE(TransactionUtils)

BOOST_AUTO_TEST_CASE(join_inputs)
{
    std::list<naivecoin::transaction::Input> const inputs{
        naivecoin::transaction::Input("some id", 23, ""),
        naivecoin::transaction::Input("some other id", 54, ""),
        naivecoin::transaction::Input("a third id", 458, "")
    };

    std::string const expected = "I{some id,23}I{some other id,54}I{a third id,458}";

    std::string const actual = naivecoin::transaction::join_inputs(inputs);

    BOOST_CHECK_EQUAL(actual, expected);
}

BOOST_AUTO_TEST_CASE(join_outputs)
{
    std::list<naivecoin::transaction::Output> const outputs{
        naivecoin::transaction::Output("12345", 7564),
        naivecoin::transaction::Output("98765", 546),
        naivecoin::transaction::Output("0001", 5436)
    };

    std::string const expected = "O{12345,7564}O{98765,546}O{0001,5436}";

    std::string const actual = naivecoin::transaction::join_outputs(outputs);

    BOOST_CHECK_EQUAL(actual, expected);
}

BOOST_AUTO_TEST_SUITE_END()
