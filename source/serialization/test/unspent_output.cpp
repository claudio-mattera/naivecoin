#include <boost/test/unit_test.hpp>

#include <naivecoin/serialization/unspent_output.h>

BOOST_AUTO_TEST_SUITE(SerializationUnspentOutput)

BOOST_AUTO_TEST_CASE(serialize)
{
    naivecoin::transaction::UnspentOutput const o1(
        "my transaction id",
        75,
        "my address",
        54
    );
    naivecoin::transaction::UnspentOutput const o2(
        "my other transaction id",
        864,
        "my other address",
        2435
    );

    std::list<naivecoin::transaction::UnspentOutput> const expected_unspent_outputs{o1, o2};

    std::string const text = naivecoin::serialization::serialize_unspent_outputs(expected_unspent_outputs);

    std::list<naivecoin::transaction::UnspentOutput> unspent_outputs = naivecoin::serialization::deserialize_unspent_outputs(text);

    BOOST_CHECK_EQUAL(unspent_outputs.size(), expected_unspent_outputs.size());

    BOOST_CHECK_EQUAL(unspent_outputs.front().transaction_id, expected_unspent_outputs.front().transaction_id);
    BOOST_CHECK_EQUAL(unspent_outputs.front().transaction_index, expected_unspent_outputs.front().transaction_index);
    BOOST_CHECK_EQUAL(unspent_outputs.front().address, expected_unspent_outputs.front().address);
    BOOST_CHECK_EQUAL(unspent_outputs.front().amount, expected_unspent_outputs.front().amount);

    BOOST_CHECK_EQUAL(unspent_outputs.back().transaction_id, expected_unspent_outputs.back().transaction_id);
    BOOST_CHECK_EQUAL(unspent_outputs.back().transaction_index, expected_unspent_outputs.back().transaction_index);
    BOOST_CHECK_EQUAL(unspent_outputs.back().address, expected_unspent_outputs.back().address);
    BOOST_CHECK_EQUAL(unspent_outputs.back().amount, expected_unspent_outputs.back().amount);
}

BOOST_AUTO_TEST_SUITE_END()
