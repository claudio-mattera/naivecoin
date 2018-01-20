#include <boost/test/unit_test.hpp>

#include <naivecoin/time/time.h>

BOOST_AUTO_TEST_SUITE(TimeTime)

BOOST_AUTO_TEST_CASE(parse_and_format)
{
    std::string const text = "2017-12-24T04:42:23Z";
    std::time_t const time = naivecoin::time::parse_timestamp(text);

    BOOST_CHECK_EQUAL(naivecoin::time::format_timestamp(time), text);
}

BOOST_AUTO_TEST_SUITE_END()
