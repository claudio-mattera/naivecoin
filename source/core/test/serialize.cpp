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

BOOST_AUTO_TEST_CASE(serialize_blockchain)
{
    naivecoin::Block const first_block = naivecoin::Block::make_block(
        0,
        "",
        naivecoin::parse_timestamp("2017-12-28T15:00:00Z"),
        "Some data 2\u2078 = 256"
    );

    naivecoin::Block const second_block = naivecoin::Block::make_block(
        1,
        "ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1",
        naivecoin::parse_timestamp("2017-12-28T16:00:00Z"),
        "Some other 2\u00B9\u2070 = 1024"
    );

    std::list<naivecoin::Block> const blockchain{first_block, second_block};

    std::string const serialized = naivecoin::serialize_blockchain(blockchain);

    std::string expected = "[\n"
        "\t{\n"
        "\t\t\"data\" : \"Some data 2⁸ = 256\",\n"
        "\t\t\"hash\" : \"5153cce46e5eeea87f0e6b7998f17e356be7ffc0\",\n"
        "\t\t\"index\" : 0,\n"
        "\t\t\"previous_hash\" : \"\",\n"
        "\t\t\"timestamp\" : \"2017-12-28T15:00:00Z\"\n"
        "\t},\n"
        "\t{\n"
        "\t\t\"data\" : \"Some other 2¹⁰ = 1024\",\n"
        "\t\t\"hash\" : \"b55d156f79f2085d432752b3f69021e7f82ea418\",\n"
        "\t\t\"index\" : 1,\n"
        "\t\t\"previous_hash\" : \"ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1\",\n"
        "\t\t\"timestamp\" : \"2017-12-28T16:00:00Z\"\n"
        "\t}\n"
        "]";

    BOOST_CHECK_EQUAL(serialized, expected);
}

BOOST_AUTO_TEST_CASE(deserialize_blockchain)
{
    std::string text = "[\n"
        "\t{\n"
        "\t\t\"data\" : \"Some data 2⁸ = 256\",\n"
        "\t\t\"hash\" : \"5153cce46e5eeea87f0e6b7998f17e356be7ffc0\",\n"
        "\t\t\"index\" : 0,\n"
        "\t\t\"previous_hash\" : \"\",\n"
        "\t\t\"timestamp\" : \"2017-12-28T15:00:00Z\"\n"
        "\t},\n"
        "\t{\n"
        "\t\t\"data\" : \"Some other 2¹⁰ = 1024\",\n"
        "\t\t\"hash\" : \"b55d156f79f2085d432752b3f69021e7f82ea418\",\n"
        "\t\t\"index\" : 1,\n"
        "\t\t\"previous_hash\" : \"ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1\",\n"
        "\t\t\"timestamp\" : \"2017-12-28T16:00:00Z\"\n"
        "\t}\n"
        "]";

    naivecoin::Block const first_block = naivecoin::Block::make_block(
        0,
        "",
        naivecoin::parse_timestamp("2017-12-28T15:00:00Z"),
        "Some data 2\u2078 = 256"
    );

    naivecoin::Block const second_block = naivecoin::Block::make_block(
        1,
        "ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1",
        naivecoin::parse_timestamp("2017-12-28T16:00:00Z"),
        "Some other 2\u00B9\u2070 = 1024"
    );

    std::list<naivecoin::Block> const expected_blockchain{first_block, second_block};

    std::list<naivecoin::Block> const blockchain = naivecoin::deserialize_blockchain(text);

    BOOST_CHECK_EQUAL(blockchain.size(), expected_blockchain.size());

    auto expected_iterator = std::begin(expected_blockchain);
    auto iterator = std::begin(blockchain);
    for (int i = 0; i < blockchain.size(); ++i) {

        BOOST_CHECK_EQUAL(iterator->index, expected_iterator->index);
        BOOST_CHECK_EQUAL(iterator->previous_hash, expected_iterator->previous_hash);
        BOOST_CHECK_EQUAL(iterator->hash, expected_iterator->hash);
        BOOST_CHECK(iterator->timestamp == expected_iterator->timestamp);
        BOOST_CHECK_EQUAL(iterator->data, expected_iterator->data);

        ++expected_iterator;
        ++iterator;
    }
}

BOOST_AUTO_TEST_SUITE_END()
