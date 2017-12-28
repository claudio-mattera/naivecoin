#include <list>
#include <iomanip>

#define BOOST_TEST_MODULE CoreBlock
#include <boost/test/unit_test.hpp>

#include <naivecoin/core/block.h>

BOOST_AUTO_TEST_CASE(compute_hash)
{
    uint64_t const index = 0;
    std::wstring const previous_hash = L"";
    std::chrono::system_clock::time_point const timestamp;
    std::wstring const data = L"Some data";

    std::wstring const hash = naivecoin::compute_hash(index, previous_hash, timestamp, data);

    std::wstring const expected_hash = L"caf893d5f68fe024a3b987b816e421b04339b7f1";

    BOOST_CHECK(hash == expected_hash);
}

BOOST_AUTO_TEST_CASE(genesis)
{
    naivecoin::Block const genesis = naivecoin::Block::genesis();

    std::tm tm = {};
    std::stringstream ss("2017-12-28T15:00:00Z");
    ss >> std::get_time(& tm, "%Y-%m-%dT%TZ");
    auto timestamp = std::chrono::system_clock::from_time_t(std::mktime(& tm));

    BOOST_CHECK_EQUAL(genesis.index, 0);
    BOOST_CHECK(genesis.previous_hash == L"");
    BOOST_CHECK(genesis.hash == L"ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1");
    BOOST_CHECK(genesis.timestamp == timestamp);
    BOOST_CHECK(genesis.data == L"");
}

BOOST_AUTO_TEST_CASE(make_block)
{
    uint64_t const index = 0;
    std::wstring const previous_hash = L"ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1";
    std::wstring const data = L"Some data";

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

    BOOST_CHECK_EQUAL(block.index, index);
    BOOST_CHECK(block.previous_hash == previous_hash);
    BOOST_CHECK(block.hash == L"d503228c47613c60efb36dc67d0a61f919223a45");
    BOOST_CHECK(block.timestamp == timestamp);
    BOOST_CHECK(block.data == data);
}

BOOST_AUTO_TEST_CASE(is_new_block_valid)
{
    naivecoin::Block const genesis = naivecoin::Block::genesis();

    BOOST_ASSERT(! naivecoin::is_new_block_valid(genesis, genesis));


    uint64_t const index = 1;
    std::wstring const previous_hash = L"ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1";
    std::wstring const data = L"Some data";

    std::tm tm = {};
    std::stringstream ss("2017-12-28T15:00:00Z");
    ss >> std::get_time(& tm, "%Y-%m-%dT%TZ");
    auto timestamp = std::chrono::system_clock::from_time_t(std::mktime(& tm));

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

    std::tm tm = {};
    std::stringstream ss("2017-12-28T15:00:00Z");
    ss >> std::get_time(& tm, "%Y-%m-%dT%TZ");
    auto timestamp = std::chrono::system_clock::from_time_t(std::mktime(& tm));

    naivecoin::Block const second_block = naivecoin::Block::make_block(
        1,
        L"ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1",
        timestamp,
        L"Some data"
    );

    naivecoin::Block const third_block = naivecoin::Block::make_block(
        2,
        L"b5961ea6760acf560cbf2395fc9ef03778f93d68",
        timestamp,
        L"Some more data"
    );

    std::list<naivecoin::Block> blockchain{genesis, second_block, third_block};

    BOOST_ASSERT(naivecoin::is_blockchain_valid(blockchain));
}
