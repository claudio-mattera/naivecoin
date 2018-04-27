#include "utils.h"

#include <fstream>
#include <sstream>
#include <regex>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

template<typename T>
void check_range(T const & value, T const & min, T const & max)
{
    if (value < min || value > max) {
        std::ostringstream stream;
        stream << value << " outside range " << min << "-" << max;
        throw po::error(stream.str());
    }
}

void check_port(uint64_t const port)
{
    check_range(port, static_cast<uint64_t>(0x1ul), static_cast<uint64_t>(0xfffful));
}

void check_socket(std::string const & value)
{
    std::regex const re("([^:]+):(\\d+)");
    std::smatch match;
    if (std::regex_match(value, match, re)) {
        uint64_t const port = std::strtoul(match[2].str().c_str(), nullptr, 10);
        check_port(port);
    } else {
        std::ostringstream stream;
        stream << value << " is not a socket in form of hostname:port";
        throw po::error(stream.str());
    }
}

std::string get_address(std::string const & private_key)
{
    std::size_t const start = 1 + private_key.find("\n");
    return private_key.substr(start, 10) + "...";
}

std::string read_file(std::string const & filename)
{
    std::ifstream stream(filename);
    return std::string(
        std::istreambuf_iterator<char>(stream),
        std::istreambuf_iterator<char>()
    );
}
