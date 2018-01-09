#include <boost/test/unit_test.hpp>

#include <string>
#include <cstdlib>

#include <naivecoin/http/http.h>

BOOST_AUTO_TEST_SUITE(HttpHttp)

BOOST_AUTO_TEST_CASE(compute_hash)
{
	std::string const request = "GET /my/path HTTP/1.0\r\n"
		"\r\n"
		"My data";

	naivecoin::Method outer_method;
	std::string outer_path;
	std::string outer_data;

	std::string const response = naivecoin::process_request(
		request,
		[&](naivecoin::Method const method, std::string const & path, std::string const & data) {
			outer_method = method;
			outer_path = path;
			outer_data = data;
			return "My response";
		}
	);

    BOOST_CHECK_EQUAL(outer_method, naivecoin::GET);
    BOOST_CHECK_EQUAL(outer_path, "/my/path");
    BOOST_CHECK_EQUAL(outer_data, "My data");

    BOOST_CHECK_EQUAL(response, "HTTP/1.0 200 Ok\r\n\r\nMy response");
}

BOOST_AUTO_TEST_SUITE_END()
