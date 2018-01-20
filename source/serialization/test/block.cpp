#include <list>
#include <iomanip>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include <naivecoin/core/block.h>
#include <naivecoin/serialization/block.h>

#include <naivecoin/time/time.h>

BOOST_AUTO_TEST_SUITE(SerializationBlock)

BOOST_AUTO_TEST_CASE(deserialize_block)
{
    std::string text = "{\n"
        "\t\"data\" : \"Some data 2\u2078 = 256\",\n"
        "\t\"difficulty\" : 0,\n"
        "\t\"hash\" : \"db38ca45847bd238c3a1eee0e17e3cc0c9aab6fd\",\n"
        "\t\"index\" : 0,\n"
        "\t\"nonce\" : 0,\n"
        "\t\"previous_hash\" : \"ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1\",\n"
        "\t\"timestamp\" : \"2017-12-28T15:00:00Z\"\n"
        "}";

    naivecoin::core::Block const block = naivecoin::serialization::deserialize_block(text);

    uint64_t const index = 0;
    std::string const previous_hash = "ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1";
    std::string const data = "Some data 2\u2078 = 256";
    uint16_t difficulty = 0;
    uint64_t nonce = 0;

    auto timestamp = naivecoin::time::parse_timestamp("2017-12-28T15:00:00Z");

    BOOST_CHECK_EQUAL(block.index, index);
    BOOST_CHECK_EQUAL(block.previous_hash, previous_hash);
    BOOST_CHECK_EQUAL(block.hash, "db38ca45847bd238c3a1eee0e17e3cc0c9aab6fd");
    BOOST_CHECK(block.timestamp == timestamp);
    BOOST_CHECK_EQUAL(block.data, data);
    BOOST_CHECK_EQUAL(block.difficulty, difficulty);
    BOOST_CHECK_EQUAL(block.nonce, nonce);
}

BOOST_AUTO_TEST_CASE(deserialize_blockchain)
{
    std::string text = "[\n"
        "\t{\n"
        "\t\t\"data\" : \"Some data 2⁸ = 256\",\n"
        "\t\t\"difficulty\" : 0,\n"
        "\t\t\"hash\" : \"bb01688103096f8389dd97460f5805dead135b2f\",\n"
        "\t\t\"index\" : 0,\n"
        "\t\t\"nonce\" : 0,\n"
        "\t\t\"previous_hash\" : \"\",\n"
        "\t\t\"timestamp\" : \"2017-12-28T15:00:00Z\"\n"
        "\t},\n"
        "\t{\n"
        "\t\t\"data\" : \"Some other 2¹⁰ = 1024\",\n"
        "\t\t\"difficulty\" : 0,\n"
        "\t\t\"hash\" : \"2c7be7cc1c7f9d92da398e0f0c9211783ab2d904\",\n"
        "\t\t\"index\" : 1,\n"
        "\t\t\"nonce\" : 0,\n"
        "\t\t\"previous_hash\" : \"bb01688103096f8389dd97460f5805dead135b2f\",\n"
        "\t\t\"timestamp\" : \"2017-12-28T16:00:00Z\"\n"
        "\t}\n"
        "]";

    naivecoin::core::Block const first_block = naivecoin::core::Block::make_block(
        0,
        "",
        naivecoin::time::parse_timestamp("2017-12-28T15:00:00Z"),
        "Some data 2\u2078 = 256",
        0,
        0
    );

    naivecoin::core::Block const second_block = naivecoin::core::Block::make_block(
        1,
        "bb01688103096f8389dd97460f5805dead135b2f",
        naivecoin::time::parse_timestamp("2017-12-28T16:00:00Z"),
        "Some other 2\u00B9\u2070 = 1024",
        0,
        0
    );

    std::list<naivecoin::core::Block> const expected_blockchain{first_block, second_block};

    std::list<naivecoin::core::Block> blockchain;
    naivecoin::serialization::deserialize_blockchain(
        std::insert_iterator(blockchain, std::begin(blockchain)),
        text
    );

    BOOST_CHECK_EQUAL(blockchain.size(), expected_blockchain.size());

    auto expected_iterator = std::begin(expected_blockchain);
    auto iterator = std::begin(blockchain);
    for (std::size_t i = 0; i < blockchain.size(); ++i) {

        BOOST_CHECK_EQUAL(iterator->index, expected_iterator->index);
        BOOST_CHECK_EQUAL(iterator->previous_hash, expected_iterator->previous_hash);
        BOOST_CHECK_EQUAL(iterator->hash, expected_iterator->hash);
        BOOST_CHECK(iterator->timestamp == expected_iterator->timestamp);
        BOOST_CHECK_EQUAL(iterator->data, expected_iterator->data);
        BOOST_CHECK_EQUAL(iterator->difficulty, expected_iterator->difficulty);
        BOOST_CHECK_EQUAL(iterator->nonce, expected_iterator->nonce);

        ++expected_iterator;
        ++iterator;
    }
}

BOOST_AUTO_TEST_SUITE_END()
