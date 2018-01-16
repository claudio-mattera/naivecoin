#include <boost/test/unit_test.hpp>

#include <naivecoin/transaction/serialize.h>

#include <naivecoin/transaction/input.h>
#include <naivecoin/transaction/output.h>
#include <naivecoin/transaction/transaction.h>

BOOST_AUTO_TEST_SUITE(TransactionSerialize)

BOOST_AUTO_TEST_CASE(serialize)
{
    naivecoin::Input const i1{"o1", 3, ""};
    naivecoin::Input const i2{"o2", 5, ""};
    naivecoin::Output const o3{"address", 33};

    naivecoin::Transaction const t{"", {i1, i2}, {o3}};
    std::list<naivecoin::Transaction> const expected_transactions{t};

    std::string const text = naivecoin::serialize_transactions(expected_transactions);

    std::list<naivecoin::Transaction> transactions = naivecoin::deserialize_transactions(text);

    BOOST_CHECK_EQUAL(transactions.size(), expected_transactions.size());
    BOOST_CHECK_EQUAL(transactions.front().id, expected_transactions.front().id);
}

BOOST_AUTO_TEST_SUITE_END()
