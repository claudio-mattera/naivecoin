#include <boost/test/unit_test.hpp>

#include <naivecoin/crypto/crypto.h>

BOOST_AUTO_TEST_SUITE(CryptoCrypto)

BOOST_AUTO_TEST_CASE(compute_hash)
{
    BOOST_CHECK_EQUAL(
        naivecoin::crypto::compute_hash(""),
        "da39a3ee5e6b4b0d3255bfef95601890afd80709"
    );

    BOOST_CHECK_EQUAL(
        naivecoin::crypto::compute_hash("non empty string"),
        "740379451ae5640e957627dd6891069a3e7731a6"
    );
}

BOOST_AUTO_TEST_CASE(generate_key_pair, *boost::unit_test::disabled())
{
    std::pair<std::string, std::string> pair = naivecoin::crypto::generate_key_pair();

    BOOST_CHECK_EQUAL(pair.first.size(), 268);
    BOOST_CHECK_EQUAL(pair.second.size(), 386);
}

BOOST_AUTO_TEST_CASE(sign_and_verify)
{
    std::pair<std::string, std::string> const pair = naivecoin::crypto::generate_key_pair();

    std::string const data = "Some data";

    std::string const signature = naivecoin::crypto::sign(data, pair.second);

    BOOST_CHECK(naivecoin::crypto::verify(data, signature, pair.first));

    std::string const other_data = "Some different data";
    std::string const other_signature = naivecoin::crypto::sign(other_data, pair.second);

    BOOST_CHECK(! naivecoin::crypto::verify(data, other_signature, pair.first));
}

BOOST_AUTO_TEST_SUITE_END()
