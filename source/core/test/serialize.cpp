#include <list>
#include <iomanip>

#include <boost/test/unit_test.hpp>

#include <naivecoin/core/block.h>
#include <naivecoin/core/serialize.h>

BOOST_AUTO_TEST_SUITE(CoreSerialize)

BOOST_AUTO_TEST_CASE(serialize_block)
{
    uint64_t const index = 0;
    std::string const previous_hash = "ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1";
    std::string const data = "Some data 2\u2078 = 256";

    std::tm tm = {};
    std::stringstream ss("2017-12-28T15:00:00Z");
    ss >> std::get_time(& tm, "%Y-%m-%dT%TZ");
    auto timestamp = std::chrono::system_clock::from_time_t(std::mktime(& tm));

    naivecoin::Block const block = naivecoin::Block::make_block(
        index,
        previous_hash,
        timestamp,
        data
    );

    std::string const serialized = naivecoin::serialize_block(block);

    std::string expected = "{\n"
        "\t\"data\" : \"Some data 2\u2078 = 256\",\n"
        "\t\"hash\" : \"59c38cb7f9984bcde56534c6894aacc13cc642ca\",\n"
        "\t\"index\" : 0,\n"
        "\t\"previous_hash\" : \"ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1\",\n"
        "\t\"timestamp\" : \"2017-12-28T14:00:00Z\"\n"
        "}";

    BOOST_CHECK_EQUAL(serialized, expected);
}

BOOST_AUTO_TEST_SUITE_END()
