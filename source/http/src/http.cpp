#include <naivecoin/http/http.h>

#include <regex>
#include <exception>

namespace {

std::string make_response(
	uint16_t const & code,
	std::string const & english,
	std::string const & data,
	uint16_t const http_version = 0
)
{
	std::ostringstream stream;
	stream << "HTTP/1." << http_version << " " << code << " " << english << "\r\n";

	if (! data.empty()) {
		stream << "Content-Length: " << data.size() << "\r\n";
		stream << "\r\n";
		stream << data;
	}

	return stream.str();
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

		std::regex const first_line_re{"^(GET|POST) (\\S+) HTTP/1.(0|1)$"};
		std::smatch first_line_match;
		if (! std::regex_match(first_line, first_line_match, first_line_re)) {
			return make_response(400, "Bad request", "Invalid HTTP request");
		}

		uint16_t const http_version = std::strtoul(first_line_match[3].str().c_str(), nullptr, 10);

		Method const method = first_line_match[1] == "GET" ? GET : POST;
		std::string const path = first_line_match[2];

		std::size_t expected_length = std::string::npos;  // By default read everything

		std::regex const header_line_re{"^([^:]+): (.+?)\r\n"};
		std::smatch header_line_match;
		std::string header_line = request.substr(2 + first_line_match.position() + first_line_match.length());
		while (std::regex_search(header_line, header_line_match, header_line_re)) {
			std::string const & header = header_line_match[1];
			std::string const & value = header_line_match[2];

			if (header == "Content-Length") {
				expected_length = std::strtoul(value.c_str(), nullptr, 10);
			}

			header_line = header_line_match.suffix();
		}

		std::size_t const start = 4 + request.find("\r\n\r\n");
		std::string const data = request.substr(start, expected_length);

		try {
			std::string const response_data = callback(method, path, data);
			return make_response(200, "Ok", response_data, http_version);
		} catch (std::exception const & e) {
			return make_response(400, "Bad request", e.what(), http_version);
		}
	} catch (std::exception const & e) {
		return make_response(500, "Server error", "");
	}
}

} // namespace naivecoin
