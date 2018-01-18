#include <list>
#include <iomanip>

#include <boost/test/unit_test.hpp>

#include <naivecoin/core/block.h>
#include <naivecoin/core/serialize.h>
#include <naivecoin/core/utils.h>

BOOST_AUTO_TEST_SUITE(CoreSerialize)

BOOST_AUTO_TEST_CASE(create_send_block_message)
{
    uint64_t const index = 0;
    std::string const previous_hash = "ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1";
    std::string const data = "Some data 2\u2078 = 256";
    uint16_t difficulty = 0;
    uint64_t nonce = 0;

    auto timestamp = naivecoin::core::parse_timestamp("2017-12-28T15:00:00Z");

    naivecoin::core::Block const block = naivecoin::core::Block::make_block(
        index,
        previous_hash,
        timestamp,
        data,
        difficulty,
        nonce
    );

    std::string const sender = "localhost:8080";

    std::string const serialized = naivecoin::core::create_send_block_message(block, sender);

    std::string expected = "{\n"
        "\t\"data\" : \n"
        "\t{\n"
        "\t\t\"data\" : \"Some data 2⁸ = 256\",\n"
        "\t\t\"difficulty\" : 0,\n"
        "\t\t\"hash\" : \"db38ca45847bd238c3a1eee0e17e3cc0c9aab6fd\",\n"
        "\t\t\"index\" : 0,\n"
        "\t\t\"nonce\" : 0,\n"
        "\t\t\"previous_hash\" : \"ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1\",\n"
        "\t\t\"timestamp\" : \"2017-12-28T15:00:00Z\"\n"
        "\t},\n"
        "\t\"message\" : \"send block\",\n"
        "\t\"sender\" : \"localhost:8080\"\n"
        "}";

    BOOST_CHECK_EQUAL(serialized, expected);
}

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

    naivecoin::core::Block const block = naivecoin::core::deserialize_block(text);

    uint64_t const index = 0;
    std::string const previous_hash = "ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1";
    std::string const data = "Some data 2\u2078 = 256";
    uint16_t difficulty = 0;
    uint64_t nonce = 0;

    auto timestamp = naivecoin::core::parse_timestamp("2017-12-28T15:00:00Z");

    BOOST_CHECK_EQUAL(block.index, index);
    BOOST_CHECK_EQUAL(block.previous_hash, previous_hash);
    BOOST_CHECK_EQUAL(block.hash, "db38ca45847bd238c3a1eee0e17e3cc0c9aab6fd");
    BOOST_CHECK(block.timestamp == timestamp);
    BOOST_CHECK_EQUAL(block.data, data);
    BOOST_CHECK_EQUAL(block.difficulty, difficulty);
    BOOST_CHECK_EQUAL(block.nonce, nonce);
}

BOOST_AUTO_TEST_CASE(create_send_blockchain_message)
{
    naivecoin::core::Block const first_block = naivecoin::core::Block::make_block(
        0,
        "",
        naivecoin::core::parse_timestamp("2017-12-28T15:00:00Z"),
        "Some data 2\u2078 = 256",
        0,
        0
    );

    naivecoin::core::Block const second_block = naivecoin::core::Block::make_block(
        1,
        "bb01688103096f8389dd97460f5805dead135b2f",
        naivecoin::core::parse_timestamp("2017-12-28T16:00:00Z"),
        "Some other 2\u00B9\u2070 = 1024",
        0,
        0
    );

    std::list<naivecoin::core::Block> const blockchain{first_block, second_block};

    std::string const sender = "localhost:8080";

    std::string const serialized = naivecoin::core::create_send_blockchain_message(blockchain, sender);

    std::string expected = "{\n"
        "\t\"data\" : \n"
        "\t[\n"
        "\t\t{\n"
        "\t\t\t\"data\" : \"Some data 2⁸ = 256\",\n"
        "\t\t\t\"difficulty\" : 0,\n"
        "\t\t\t\"hash\" : \"bb01688103096f8389dd97460f5805dead135b2f\",\n"
        "\t\t\t\"index\" : 0,\n"
        "\t\t\t\"nonce\" : 0,\n"
        "\t\t\t\"previous_hash\" : \"\",\n"
        "\t\t\t\"timestamp\" : \"2017-12-28T15:00:00Z\"\n"
        "\t\t},\n"
        "\t\t{\n"
        "\t\t\t\"data\" : \"Some other 2¹⁰ = 1024\",\n"
        "\t\t\t\"difficulty\" : 0,\n"
        "\t\t\t\"hash\" : \"2c7be7cc1c7f9d92da398e0f0c9211783ab2d904\",\n"
        "\t\t\t\"index\" : 1,\n"
        "\t\t\t\"nonce\" : 0,\n"
        "\t\t\t\"previous_hash\" : \"bb01688103096f8389dd97460f5805dead135b2f\",\n"
        "\t\t\t\"timestamp\" : \"2017-12-28T16:00:00Z\"\n"
        "\t\t}\n"
        "\t],\n"
        "\t\"message\" : \"send blockchain\",\n"
        "\t\"sender\" : \"localhost:8080\"\n"
        "}";

    BOOST_CHECK_EQUAL(serialized, expected);
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
        naivecoin::core::parse_timestamp("2017-12-28T15:00:00Z"),
        "Some data 2\u2078 = 256",
        0,
        0
    );

    naivecoin::core::Block const second_block = naivecoin::core::Block::make_block(
        1,
        "bb01688103096f8389dd97460f5805dead135b2f",
        naivecoin::core::parse_timestamp("2017-12-28T16:00:00Z"),
        "Some other 2\u00B9\u2070 = 1024",
        0,
        0
    );

    std::list<naivecoin::core::Block> const expected_blockchain{first_block, second_block};

    std::list<naivecoin::core::Block> const blockchain = naivecoin::core::deserialize_blockchain(text);

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

BOOST_AUTO_TEST_CASE(create_query_latest_block_message)
{
    std::string const sender = "localhost:8080";

    std::string const serialized = naivecoin::core::create_query_latest_block_message(sender);

    std::string expected = "{\n"
        "\t\"data\" : null,\n"
        "\t\"message\" : \"query latest block\",\n"
        "\t\"sender\" : \"localhost:8080\"\n"
        "}";

    BOOST_CHECK_EQUAL(serialized, expected);
}

BOOST_AUTO_TEST_CASE(create_query_blockchain_message)
{
    std::string const sender = "localhost:8080";

    std::string const serialized = naivecoin::core::create_query_blockchain_message(sender);

    std::string expected = "{\n"
        "\t\"data\" : null,\n"
        "\t\"message\" : \"query blockchain\",\n"
        "\t\"sender\" : \"localhost:8080\"\n"
        "}";

    BOOST_CHECK_EQUAL(serialized, expected);
}

BOOST_AUTO_TEST_CASE(process_message_send_block)
{
    uint64_t const index = 0;
    std::string const previous_hash = "ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1";
    std::string const data = "Some data 2\u2078 = 256";
    uint16_t difficulty = 0;
    uint64_t nonce = 0;

    auto timestamp = naivecoin::core::parse_timestamp("2017-12-28T15:00:00Z");

    naivecoin::core::Block const original_block = naivecoin::core::Block::make_block(
        index,
        previous_hash,
        timestamp,
        data,
        difficulty,
        nonce
    );

    std::string const original_sender = "localhost:8080";

    std::string const message = naivecoin::core::create_send_block_message(original_block, original_sender);

    auto process_send_block_message = [&original_sender, &original_block](naivecoin::core::Block const & block, std::string const & sender){
        BOOST_CHECK(sender == original_sender);

        BOOST_CHECK_EQUAL(block.index, original_block.index);
        BOOST_CHECK_EQUAL(block.previous_hash, original_block.previous_hash);
        BOOST_CHECK_EQUAL(block.hash, original_block.hash);
        BOOST_CHECK(block.timestamp == original_block.timestamp);
        BOOST_CHECK_EQUAL(block.data, original_block.data);
        BOOST_CHECK_EQUAL(block.difficulty, original_block.difficulty);
        BOOST_CHECK_EQUAL(block.nonce, original_block.nonce);
    };
    auto process_send_blockchain_message = [](std::list<naivecoin::core::Block> const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_query_latest_block_message = [](std::string const &){
        BOOST_CHECK(false);
    };
    auto process_query_blockchain_message = [](std::string const &){
        BOOST_CHECK(false);
    };
    auto process_unknown_message = [](std::string const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_invalid_message = [](std::string const &){
        BOOST_CHECK(false);
    };

    naivecoin::core::process_message(
        message,
        process_send_block_message,
        process_send_blockchain_message,
        process_query_latest_block_message,
        process_query_blockchain_message,
        process_unknown_message,
        process_invalid_message
    );
}

BOOST_AUTO_TEST_CASE(process_message_send_blockchain)
{
    std::string const original_sender = "localhost:8080";

    naivecoin::core::Block const first_block = naivecoin::core::Block::make_block(
        0,
        "",
        naivecoin::core::parse_timestamp("2017-12-28T15:00:00Z"),
        "Some data 2\u2078 = 256",
        0,
        0
    );

    naivecoin::core::Block const second_block = naivecoin::core::Block::make_block(
        1,
        "bb01688103096f8389dd97460f5805dead135b2f",
        naivecoin::core::parse_timestamp("2017-12-28T16:00:00Z"),
        "Some other 2\u00B9\u2070 = 1024",
        0,
        0
    );

    std::list<naivecoin::core::Block> const original_blockchain{first_block, second_block};

    std::string const message = naivecoin::core::create_send_blockchain_message(original_blockchain, original_sender);

    auto process_send_block_message = [](naivecoin::core::Block const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_send_blockchain_message = [&original_sender, &original_blockchain](std::list<naivecoin::core::Block> const & blockchain, std::string const & sender){
        BOOST_CHECK(sender == original_sender);

        auto expected_iterator = std::begin(original_blockchain);
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
    };
    auto process_query_latest_block_message = [](std::string const &){
        BOOST_CHECK(false);
    };
    auto process_query_blockchain_message = [](std::string const &){
        BOOST_CHECK(false);
    };
    auto process_unknown_message = [](std::string const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_invalid_message = [](std::string const &){
        BOOST_CHECK(false);
    };

    naivecoin::core::process_message(
        message,
        process_send_block_message,
        process_send_blockchain_message,
        process_query_latest_block_message,
        process_query_blockchain_message,
        process_unknown_message,
        process_invalid_message
    );
}

BOOST_AUTO_TEST_CASE(process_message_query_latest_block)
{
    std::string const original_sender = "localhost:8080";

    std::string const message = naivecoin::core::create_query_latest_block_message(original_sender);

    auto process_send_block_message = [](naivecoin::core::Block const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_send_blockchain_message = [](std::list<naivecoin::core::Block> const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_query_latest_block_message = [&original_sender](std::string const & sender){
        BOOST_CHECK(sender == original_sender);
    };
    auto process_query_blockchain_message = [](std::string const &){
        BOOST_CHECK(false);
    };
    auto process_unknown_message = [](std::string const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_invalid_message = [](std::string const &){
        BOOST_CHECK(false);
    };

    naivecoin::core::process_message(
        message,
        process_send_block_message,
        process_send_blockchain_message,
        process_query_latest_block_message,
        process_query_blockchain_message,
        process_unknown_message,
        process_invalid_message
    );
}

BOOST_AUTO_TEST_CASE(process_message_query_blockchain)
{
    std::string const original_sender = "localhost:8080";

    std::string const message = naivecoin::core::create_query_blockchain_message(original_sender);

    auto process_send_block_message = [](naivecoin::core::Block const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_send_blockchain_message = [](std::list<naivecoin::core::Block> const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_query_latest_block_message = [](std::string const &){
        BOOST_CHECK(false);
    };
    auto process_query_blockchain_message = [&original_sender](std::string const & sender){
        BOOST_CHECK(sender == original_sender);
    };
    auto process_unknown_message = [](std::string const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_invalid_message = [](std::string const &){
        BOOST_CHECK(false);
    };

    naivecoin::core::process_message(
        message,
        process_send_block_message,
        process_send_blockchain_message,
        process_query_latest_block_message,
        process_query_blockchain_message,
        process_unknown_message,
        process_invalid_message
    );
}

BOOST_AUTO_TEST_CASE(process_unknown_message)
{
    std::string const message = "{\"message\": \"fhllierjgijdfjl\", \"sender\": \"localhost:8080\"}";

    auto process_send_block_message = [](naivecoin::core::Block const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_send_blockchain_message = [](std::list<naivecoin::core::Block> const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_query_latest_block_message = [](std::string const &){
        BOOST_CHECK(false);
    };
    auto process_query_blockchain_message = [](std::string const &){
        BOOST_CHECK(false);
    };
    auto process_unknown_message = [](std::string const & message, std::string const & sender){
        BOOST_CHECK(sender == "localhost:8080");
        BOOST_CHECK(message == "fhllierjgijdfjl");
    };
    auto process_invalid_message = [](std::string const &){
        BOOST_CHECK(false);
    };

    naivecoin::core::process_message(
        message,
        process_send_block_message,
        process_send_blockchain_message,
        process_query_latest_block_message,
        process_query_blockchain_message,
        process_unknown_message,
        process_invalid_message
    );
}

BOOST_AUTO_TEST_CASE(process_invalid_message)
{
    std::string const message = "[]";

    auto process_send_block_message = [](naivecoin::core::Block const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_send_blockchain_message = [](std::list<naivecoin::core::Block> const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_query_latest_block_message = [](std::string const &){
        BOOST_CHECK(false);
    };
    auto process_query_blockchain_message = [](std::string const &){
        BOOST_CHECK(false);
    };
    auto process_unknown_message = [](std::string const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_invalid_message = [](std::string const &){
        BOOST_CHECK(true);
    };

    naivecoin::core::process_message(
        message,
        process_send_block_message,
        process_send_blockchain_message,
        process_query_latest_block_message,
        process_query_blockchain_message,
        process_unknown_message,
        process_invalid_message
    );
}

BOOST_AUTO_TEST_CASE(process_invalid_json_message)
{
    std::string const message = "few9n8y438v5t3nl86rtkretgiu";

    auto process_send_block_message = [](naivecoin::core::Block const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_send_blockchain_message = [](std::list<naivecoin::core::Block> const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_query_latest_block_message = [](std::string const &){
        BOOST_CHECK(false);
    };
    auto process_query_blockchain_message = [](std::string const &){
        BOOST_CHECK(false);
    };
    auto process_unknown_message = [](std::string const &, std::string const &){
        BOOST_CHECK(false);
    };
    auto process_invalid_message = [](std::string const &){
        BOOST_CHECK(true);
    };

    naivecoin::core::process_message(
        message,
        process_send_block_message,
        process_send_blockchain_message,
        process_query_latest_block_message,
        process_query_blockchain_message,
        process_unknown_message,
        process_invalid_message
    );
}

BOOST_AUTO_TEST_SUITE_END()
