#include <list>
#include <iomanip>

#include <boost/test/unit_test.hpp>

#include <naivecoin/core/block.h>
#include <naivecoin/core/utils.h>

BOOST_AUTO_TEST_SUITE(CoreBlock)

BOOST_AUTO_TEST_CASE(compute_hash)
{
    uint64_t const index = 0;
    std::string const previous_hash = "";
    std::time_t const timestamp = naivecoin::parse_timestamp("1970-01-01T00:00:00Z");
    std::string const data = "Some data";

    std::string const hash = naivecoin::compute_hash(index, previous_hash, timestamp, data);

    std::string const expected_hash = "caf893d5f68fe024a3b987b816e421b04339b7f1";

    BOOST_CHECK_EQUAL(hash, expected_hash);
}

BOOST_AUTO_TEST_CASE(genesis)
{
    naivecoin::Block const genesis = naivecoin::Block::genesis();

    auto timestamp = naivecoin::parse_timestamp("2017-12-28T15:00:00Z");

    BOOST_CHECK_EQUAL(genesis.index, 0);
    BOOST_CHECK_EQUAL(genesis.previous_hash, "");
    BOOST_CHECK_EQUAL(genesis.hash, "3cf59f93577dbe819ae24cd73beffc4e7efabe65");
    BOOST_CHECK(genesis.timestamp == timestamp);
    BOOST_CHECK_EQUAL(genesis.data, "");
}

BOOST_AUTO_TEST_CASE(make_block)
{
    uint64_t const index = 0;
    std::string const previous_hash = "3cf59f93577dbe819ae24cd73beffc4e7efabe65";
    std::string const data = "Some data";

    auto timestamp = naivecoin::parse_timestamp("2017-12-28T15:00:00Z");

    naivecoin::Block const block = naivecoin::Block::make_block(
        index,
        previous_hash,
        timestamp,
        data
    );

    BOOST_CHECK_EQUAL(block.index, index);
    BOOST_CHECK_EQUAL(block.previous_hash, previous_hash);
    BOOST_CHECK_EQUAL(block.hash, "f8ab7f441f391f00a7393f51a188139e36056651");
    BOOST_CHECK(block.timestamp == timestamp);
    BOOST_CHECK_EQUAL(block.data, data);
}

BOOST_AUTO_TEST_CASE(is_new_block_valid)
{
    naivecoin::Block const genesis = naivecoin::Block::genesis();

    BOOST_ASSERT(! naivecoin::is_new_block_valid(genesis, genesis));


    uint64_t const index = 1;
    std::string const previous_hash = "3cf59f93577dbe819ae24cd73beffc4e7efabe65";
    std::string const data = "Some data";

    auto timestamp = naivecoin::parse_timestamp("2017-12-28T15:00:00Z");

    naivecoin::Block const new_block = naivecoin::Block::make_block(
        index,
        previous_hash,
        timestamp,
        data
    );

    BOOST_ASSERT(naivecoin::is_new_block_valid(new_block, genesis));


    naivecoin::Block const new_bad_block_1 = naivecoin::Block::make_block(
        index + 3,
        previous_hash,
        timestamp,
        data
    );

    BOOST_ASSERT(! naivecoin::is_new_block_valid(new_bad_block_1, genesis));
}

BOOST_AUTO_TEST_CASE(is_blockchain_valid)
{
    naivecoin::Block const genesis = naivecoin::Block::genesis();

    auto timestamp = naivecoin::parse_timestamp("2017-12-28T15:00:00Z");

    naivecoin::Block const second_block = naivecoin::Block::make_block(
        1,
        "3cf59f93577dbe819ae24cd73beffc4e7efabe65",
        timestamp,
        "Some data"
    );

    naivecoin::Block const third_block = naivecoin::Block::make_block(
        2,
        "249feacf4268097d993ccb03cc21d4551b7a3a2b",
        timestamp,
        "Some more data"
    );

    std::list<naivecoin::Block> blockchain{genesis, second_block, third_block};

    BOOST_ASSERT(naivecoin::is_blockchain_valid(blockchain));
}

BOOST_AUTO_TEST_SUITE_END()
