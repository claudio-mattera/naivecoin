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
