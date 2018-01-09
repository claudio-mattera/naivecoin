#include <naivecoin/http/http.h>

#include <regex>
#include <exception>

namespace {

std::string make_response(std::string const & code, std::string const & data)
{
	return code + std::string("\r\n\r\n") + data;
}

} // unnamed namespace

namespace naivecoin {

std::string process_request(
	std::string const & request,
	std::function<std::string(Method const, std::string const &, std::string const &)> callback
)
{
	try {
		std::size_t const first_newline_pos = request.find("\r\n");
		std::string const & first_line = request.substr(0, first_newline_pos);

		std::regex const first_line_re{"^(GET|POST) (\\S+) HTTP/1.0$"};
		std::smatch first_line_match;
		if (! std::regex_match(first_line, first_line_match, first_line_re)) {
			return make_response("HTTP/1.0 400 Bad request", "Invalid HTTP request");
		}

		Method const method = first_line_match[1] == "GET" ? GET : POST;
		std::string const path = first_line_match[2];
		std::size_t const start = 4 + request.find("\r\n\r\n");
		std::string const data = request.substr(start);

		try {
			std::string const response_data = callback(method, path, data);
			return make_response("HTTP/1.0 200 Ok", response_data);
		} catch (std::exception const & e) {
			return make_response("HTTP/1.0 400 Bad request", e.what());
		}
	} catch (std::exception const & e) {
		return make_response("HTTP/1.0 500 Server error", "");
	}
}

} // namespace naivecoin
