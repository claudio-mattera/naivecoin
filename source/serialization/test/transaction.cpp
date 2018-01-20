#include <boost/test/unit_test.hpp>

#include <naivecoin/serialization/transaction.h>

#include <naivecoin/transaction/input.h>
#include <naivecoin/transaction/output.h>
#include <naivecoin/transaction/transaction.h>

BOOST_AUTO_TEST_SUITE(SerializationTransaction)

BOOST_AUTO_TEST_CASE(serialize)
{
    naivecoin::transaction::Input const i1{"o1", 3, ""};
    naivecoin::transaction::Input const i2{"o2", 5, ""};
    naivecoin::transaction::Output const o3{"address", 33};

    naivecoin::transaction::Transaction const t{"", {i1, i2}, {o3}};
    std::list<naivecoin::transaction::Transaction> const expected_transactions{t};

    std::string const text = naivecoin::serialization::serialize_transactions(expected_transactions);

    std::list<naivecoin::transaction::Transaction> transactions = naivecoin::serialization::deserialize_transactions(text);

    BOOST_CHECK_EQUAL(transactions.size(), expected_transactions.size());
    BOOST_CHECK_EQUAL(transactions.front().id, expected_transactions.front().id);
}

BOOST_AUTO_TEST_SUITE_END()
