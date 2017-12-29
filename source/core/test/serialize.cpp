#include <list>
#include <iomanip>

#include <boost/test/unit_test.hpp>

#include <naivecoin/core/block.h>
#include <naivecoin/core/serialize.h>
#include <naivecoin/core/utils.h>

BOOST_AUTO_TEST_SUITE(CoreSerialize)

BOOST_AUTO_TEST_CASE(serialize_block)
{
    uint64_t const index = 0;
    std::string const previous_hash = "ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1";
    std::string const data = "Some data 2\u2078 = 256";

    auto timestamp = naivecoin::parse_timestamp("2017-12-28T15:00:00Z");

    naivecoin::Block const block = naivecoin::Block::make_block(
        index,
        previous_hash,
        timestamp,
        data
    );

    std::string const serialized = naivecoin::serialize_block(block);

    std::string expected = "{\n"
        "\t\"data\" : \"Some data 2\u2078 = 256\",\n"
        "\t\"hash\" : \"e0949ea4b488acfa792e6be0900bca6fc6f7eee5\",\n"
        "\t\"index\" : 0,\n"
        "\t\"previous_hash\" : \"ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1\",\n"
        "\t\"timestamp\" : \"2017-12-28T15:00:00Z\"\n"
        "}";

    BOOST_CHECK_EQUAL(serialized, expected);
}

BOOST_AUTO_TEST_CASE(deserialize_block)
{
    std::string text = "{\n"
        "\t\"data\" : \"Some data 2\u2078 = 256\",\n"
        "\t\"hash\" : \"e0949ea4b488acfa792e6be0900bca6fc6f7eee5\",\n"
        "\t\"index\" : 0,\n"
        "\t\"previous_hash\" : \"ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1\",\n"
        "\t\"timestamp\" : \"2017-12-28T15:00:00Z\"\n"
        "}";

    naivecoin::Block const block = naivecoin::deserialize_block(text);

    uint64_t const index = 0;
    std::string const previous_hash = "ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1";
    std::string const data = "Some data 2\u2078 = 256";

    auto timestamp = naivecoin::parse_timestamp("2017-12-28T15:00:00Z");

    BOOST_CHECK_EQUAL(block.index, index);
    BOOST_CHECK_EQUAL(block.previous_hash, previous_hash);
    BOOST_CHECK_EQUAL(block.hash, "e0949ea4b488acfa792e6be0900bca6fc6f7eee5");
    BOOST_CHECK(block.timestamp == timestamp);
    BOOST_CHECK_EQUAL(block.data, data);
}

BOOST_AUTO_TEST_SUITE_END()
