#include <boost/test/unit_test.hpp>

#include <naivecoin/transaction/utils.h>

BOOST_AUTO_TEST_SUITE(TransactionUtils)

BOOST_AUTO_TEST_CASE(join_inputs)
{
    std::list<naivecoin::Input> const inputs{
        naivecoin::Input("some id", 23, ""),
        naivecoin::Input("some other id", 54, ""),
        naivecoin::Input("a third id", 458, "")
    };

    std::string const expected = "I{some id,23}I{some other id,54}I{a third id,458}";

    std::string const actual = naivecoin::join_inputs(inputs);

    BOOST_CHECK_EQUAL(actual, expected);
}

BOOST_AUTO_TEST_CASE(join_outputs)
{
    std::list<naivecoin::Output> const outputs{
        naivecoin::Output("12345", 7564),
        naivecoin::Output("98765", 546),
        naivecoin::Output("0001", 5436)
    };

    std::string const expected = "O{12345,7564}O{98765,546}O{0001,5436}";

    std::string const actual = naivecoin::join_outputs(outputs);

    BOOST_CHECK_EQUAL(actual, expected);
}

BOOST_AUTO_TEST_SUITE_END()
