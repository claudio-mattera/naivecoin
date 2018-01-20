#include <list>
#include <array>

#include <boost/test/unit_test.hpp>

#include <naivecoin/core/utils.h>

BOOST_AUTO_TEST_SUITE(CoreUtils)

BOOST_AUTO_TEST_CASE(hex_to_binary_1)
{
    std::string const hex = "b";
    std::array<bool, 4> expected{true, false, true, true};
    std::array<bool, 4> actual = naivecoin::core::hex_to_binary<1>(hex);

    BOOST_CHECK(actual == expected);
}

BOOST_AUTO_TEST_CASE(hex_to_binary_2)
{
    std::string const hex = "4";
    std::array<bool, 4> expected{false, true, false, false};
    std::array<bool, 4> actual = naivecoin::core::hex_to_binary<1>(hex);

    BOOST_CHECK(actual == expected);
}

BOOST_AUTO_TEST_CASE(hex_to_binary_3)
{
    std::string const hex = "42b4c9af";
    std::array<bool, 32> expected{
        false, true, false, false,
        false, false, true, false,
        true, false, true, true,
        false, true, false, false,
        true, true, false, false,
        true, false, false, true,
        true, false, true, false,
        true, true, true, true
    };
    std::array<bool, 32> actual = naivecoin::core::hex_to_binary<8>(hex);

    BOOST_CHECK(actual == expected);
}

BOOST_AUTO_TEST_CASE(replace)
{
    std::string const input = "hello world";
    std::string const actual = naivecoin::core::replace(input, "ll", "kk");
    std::string const expected = "hekko world";

    BOOST_CHECK_EQUAL(actual, expected);
}

BOOST_AUTO_TEST_SUITE_END()
