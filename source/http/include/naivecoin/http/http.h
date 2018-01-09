
#pragma once

#ifndef HTTP_H
#define HTTP_H

#include <string>
#include <functional>

namespace naivecoin {

enum Method {
	GET = 1,
	POST = 2
};

std::string process_request(
	std::string const & request,
	std::function<std::string(Method const, std::string const &, std::string const &)> callback
);

} // namespace naivecoin

#endif // HTTP_H
