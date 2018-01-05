#include <list>
#include <array>

#include <boost/test/unit_test.hpp>

#include <naivecoin/crypto/crypto.h>

BOOST_AUTO_TEST_SUITE(CryptoCrypto)

BOOST_AUTO_TEST_CASE(compute_hash)
{
    BOOST_CHECK_EQUAL(
        naivecoin::compute_hash(""),
        "da39a3ee5e6b4b0d3255bfef95601890afd80709"
    );

    BOOST_CHECK_EQUAL(
        naivecoin::compute_hash("non empty string"),
        "740379451ae5640e957627dd6891069a3e7731a6"
    );
}

BOOST_AUTO_TEST_CASE(generate_key_pair)
{
    std::pair<std::string, std::string> pair = naivecoin::generate_key_pair();

    BOOST_CHECK_EQUAL(pair.first.size(), 268);
    BOOST_CHECK_EQUAL(pair.second.size(), 384);
}

BOOST_AUTO_TEST_SUITE_END()
