#include <boost/test/unit_test.hpp>

#include <naivecoin/time/time.h>

BOOST_AUTO_TEST_SUITE(TimeTime)

BOOST_AUTO_TEST_CASE(parse)
{
    std::string const text = "2017-12-24T04:42:23Z";
    naivecoin::time::instant const parsed_instant = naivecoin::time::parse_timestamp(text);

    boost::gregorian::date date(2017, 12, 24);
    boost::posix_time::ptime expected(date, boost::posix_time::hours(4) + boost::posix_time::minutes(42) + boost::posix_time::seconds(23));

    BOOST_CHECK_EQUAL(parsed_instant, expected);
}

BOOST_AUTO_TEST_CASE(format)
{
    boost::gregorian::date const date(2017, 12, 24);
    boost::posix_time::ptime const datetime(date, boost::posix_time::hours(4) + boost::posix_time::minutes(42) + boost::posix_time::seconds(23));

    std::string const expected = "2017-12-24T04:42:23Z";

    BOOST_CHECK_EQUAL(naivecoin::time::format_timestamp(datetime), expected);
}

BOOST_AUTO_TEST_CASE(parse_and_format)
{
    std::string const text = "2017-12-24T04:42:23Z";
    naivecoin::time::instant const time = naivecoin::time::parse_timestamp(text);

    BOOST_CHECK_EQUAL(naivecoin::time::format_timestamp(time), text);
}

BOOST_AUTO_TEST_SUITE_END()
