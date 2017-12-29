#include <list>
#include <iomanip>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <naivecoin/core/block.h>
#include <naivecoin/core/serialize.h>

BOOST_AUTO_TEST_SUITE(CoreSerialize)

BOOST_AUTO_TEST_CASE(serialize_block)
{
    uint64_t const index = 0;
    std::wstring const previous_hash = L"ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1";
    std::wstring const data = L"Some data 2\u2078 = 256";

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

    std::wstring const serialized = naivecoin::serialize_block(block);

    std::wcout << data << L'\n';
    std::wcout << serialized << L'\n';

    std::wstring expected = L"{\n"
        "     \"data\" : \"Some data 2x = 256\",\n"
        "     \"hash\" : \"d85ebd21edec93452860d61c14512eeaa42d4819\",\n"
        "     \"index\" : 0,\n"
        "     \"previous_hash\" : \"ac0c62f1871b2bda6c28af2a12f9cc1487b2d2b1\",\n"
        "     \"timestamp\" : \"2017-12-28T14:00:00Z\"\n"
        "}";
    std::wcout << expected << L'\n';

    BOOST_CHECK(serialized == expected);
}

BOOST_AUTO_TEST_SUITE_END()
