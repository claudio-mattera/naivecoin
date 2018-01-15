#include <list>
#include <iomanip>

#include <boost/test/unit_test.hpp>

#include <naivecoin/core/block.h>
#include <naivecoin/core/utils.h>

#include <naivecoin/crypto/crypto.h>

BOOST_AUTO_TEST_SUITE(CoreBlock)

BOOST_AUTO_TEST_CASE(compute_block_hash)
{
    uint64_t const index = 0;
    std::string const previous_hash = "";
    std::time_t const timestamp = naivecoin::parse_timestamp("1970-01-01T00:00:00Z");
    std::string const data = "Some data";
    uint16_t difficulty = 0;
    uint64_t nonce = 0;

    std::string const hash = naivecoin::compute_block_hash(index, previous_hash, timestamp, data, difficulty, nonce);

    std::string const expected_hash = "25b14367acd2fee29d0b2a8ed4603297f5703a59";

    BOOST_CHECK_EQUAL(hash, expected_hash);
}

BOOST_AUTO_TEST_CASE(genesis)
{
    naivecoin::Block const genesis = naivecoin::Block::genesis();

    auto timestamp = naivecoin::parse_timestamp("2017-12-28T15:00:00Z");

    BOOST_CHECK_EQUAL(genesis.index, 0);
    BOOST_CHECK_EQUAL(genesis.previous_hash, "");
    BOOST_CHECK_EQUAL(genesis.hash, "2366930026a1831f1490ea762d134ffa03f675dd");
    BOOST_CHECK(genesis.timestamp == timestamp);
    BOOST_CHECK_EQUAL(genesis.data, "");
    BOOST_CHECK_EQUAL(genesis.difficulty, 0);
    BOOST_CHECK_EQUAL(genesis.nonce, 0);
}

BOOST_AUTO_TEST_CASE(make_block)
{
    uint64_t const index = 0;
    std::string const previous_hash = "2366930026a1831f1490ea762d134ffa03f675dd";
    std::string const data = "Some data";
    uint16_t difficulty = 0;
    uint64_t nonce = 0;

    auto timestamp = naivecoin::parse_timestamp("2017-12-28T15:00:00Z");

    naivecoin::Block const block = naivecoin::Block::make_block(
        index,
        previous_hash,
        timestamp,
        data,
        difficulty,
        nonce
    );

    BOOST_CHECK_EQUAL(block.index, index);
    BOOST_CHECK_EQUAL(block.previous_hash, previous_hash);
    BOOST_CHECK_EQUAL(block.hash, "d8cb8740f79e3447022eed5ef7640c5daca5f65a");
    BOOST_CHECK(block.timestamp == timestamp);
    BOOST_CHECK_EQUAL(block.data, data);
    BOOST_CHECK_EQUAL(block.difficulty, 0);
    BOOST_CHECK_EQUAL(block.nonce, 0);
}

BOOST_AUTO_TEST_CASE(is_new_block_valid)
{
    naivecoin::Block const genesis = naivecoin::Block::genesis();

    BOOST_ASSERT(! naivecoin::is_new_block_valid(genesis, genesis));


    uint64_t const index = 1;
    std::string const previous_hash = "2366930026a1831f1490ea762d134ffa03f675dd";
    std::string const data = "Some data";
    uint16_t difficulty = 0;
    uint64_t nonce = 0;

    auto timestamp = naivecoin::parse_timestamp("2017-12-28T15:00:00Z");

    naivecoin::Block const new_block = naivecoin::Block::make_block(
        index,
        previous_hash,
        timestamp,
        data,
        difficulty,
        nonce
    );

    BOOST_ASSERT(naivecoin::is_new_block_valid(new_block, genesis));


    naivecoin::Block const new_bad_block_1 = naivecoin::Block::make_block(
        index + 3,
        previous_hash,
        timestamp,
        data,
        difficulty,
        nonce
    );

    BOOST_ASSERT(! naivecoin::is_new_block_valid(new_bad_block_1, genesis));
}

BOOST_AUTO_TEST_CASE(is_blockchain_valid)
{
    naivecoin::Block const genesis = naivecoin::Block::genesis();

    auto timestamp = naivecoin::parse_timestamp("2017-12-28T15:00:00Z");

    naivecoin::Block const second_block = naivecoin::Block::make_block(
        1,
        "2366930026a1831f1490ea762d134ffa03f675dd",
        timestamp,
        "Some data",
        0,
        0
    );

    naivecoin::Block const third_block = naivecoin::Block::make_block(
        2,
        "a0f5e0cbb1b06ff17f236b90168db9fb1ecd4278",
        timestamp,
        "Some more data",
        0,
        0
    );

    std::list<naivecoin::Block> blockchain{genesis, second_block, third_block};

    BOOST_ASSERT(naivecoin::is_blockchain_valid(blockchain));
}

BOOST_AUTO_TEST_CASE(hash_matches_difficulty)
{
    uint16_t const difficulty_1 = 4;
    std::string const hash_1_1 = "08f3a3ee5e6b4b0d3255bfef95601890afd80709";
    std::string const hash_1_2 = "16bca3ee5e6b4b0d3255bfef95601890afd80709";

    BOOST_ASSERT(naivecoin::hash_matches_difficulty(hash_1_1, difficulty_1));
    BOOST_ASSERT(! naivecoin::hash_matches_difficulty(hash_1_2, difficulty_1));

    uint16_t const difficulty_2 = 8;
    std::string const hash_2_1 = "0027a3ee5e6b4b0d3255bfef95601890afd80709";
    std::string const hash_2_2 = "05c2a3ee5e6b4b0d3255bfef95601890afd80709";

    BOOST_ASSERT(naivecoin::hash_matches_difficulty(hash_2_1, difficulty_2));
    BOOST_ASSERT(! naivecoin::hash_matches_difficulty(hash_2_2, difficulty_2));
}

BOOST_AUTO_TEST_CASE(compute_cumulative_difficulty)
{
    naivecoin::Block const first_block = naivecoin::Block::make_block(
        0,
        "",
        naivecoin::parse_timestamp("2017-12-28T15:00:00Z"),
        "Some data 2\u2078 = 256",
        4,
        0
    );

    naivecoin::Block const second_block = naivecoin::Block::make_block(
        1,
        "bb01688103096f8389dd97460f5805dead135b2f",
        naivecoin::parse_timestamp("2017-12-28T16:00:00Z"),
        "Some other 2\u00B9\u2070 = 1024",
        6,
        0
    );

    std::list<naivecoin::Block> const blockchain{first_block, second_block};

    uint64_t const expected_difficulty = 16 + 64;
    uint64_t const actual_difficulty = naivecoin::compute_cumulative_difficulty(blockchain);

    BOOST_CHECK_EQUAL(actual_difficulty, expected_difficulty);
}

BOOST_AUTO_TEST_SUITE_END()
