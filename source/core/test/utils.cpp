#include <list>
#include <array>

#include <boost/test/unit_test.hpp>

#include <naivecoin/core/utils.h>

BOOST_AUTO_TEST_SUITE(CoreUtils)

BOOST_AUTO_TEST_CASE(parse_and_format_timestamp)
{
    auto first_timestamp = naivecoin::parse_timestamp("2017-12-28T15:00:00Z");
    std::string first_string = naivecoin::format_timestamp(first_timestamp);
    auto second_timestamp = naivecoin::parse_timestamp(first_string);
    std::string second_string = naivecoin::format_timestamp(second_timestamp);
    auto third_timestamp = naivecoin::parse_timestamp(second_string);
    std::string third_string = naivecoin::format_timestamp(third_timestamp);
    auto fourth_timestamp = naivecoin::parse_timestamp(third_string);
    std::string fourth_string = naivecoin::format_timestamp(fourth_timestamp);

    BOOST_CHECK_EQUAL(first_string, fourth_string);
}

BOOST_AUTO_TEST_CASE(hex_to_binary_1)
{
    std::string const hex = "b";
    std::array<bool, 4> expected{true, false, true, true};
    std::array<bool, 4> actual = naivecoin::hex_to_binary<1>(hex);

    BOOST_CHECK(actual == expected);
}

BOOST_AUTO_TEST_CASE(hex_to_binary_2)
{
    std::string const hex = "4";
    std::array<bool, 4> expected{false, true, false, false};
    std::array<bool, 4> actual = naivecoin::hex_to_binary<1>(hex);

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
    std::array<bool, 32> actual = naivecoin::hex_to_binary<8>(hex);

    BOOST_CHECK(actual == expected);
}

BOOST_AUTO_TEST_SUITE_END()
