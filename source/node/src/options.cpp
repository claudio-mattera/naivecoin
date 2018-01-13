#include "options.h"

#include <cstdlib>
#include <iostream>
#include <regex>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace {

void show_help(
    po::options_description const & description,
    std::string const & topic = "",
    int const error_code = EXIT_SUCCESS
)
{
    std::cout << description << '\n';
    if (topic != "") {
        std::cout << "Unknown topic " << topic << '\n';
    }
    std::exit(error_code);
}

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
    check_range(port, 0x1ul, 0xfffful);
}

void check_socket(std::string const & value)
{
    // passing -1 as the submatch index parameter performs splitting
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

} // unnamed namespace

namespace naivecoin {

boost::program_options::variables_map process_program_options(int const argc, char const * const argv[])
{
    po::options_description description("Usage");
    description.add_options()
        (
            "help,h",
            po::value<std::string>()
                ->implicit_value("")
                ->notifier(
                    [& description](std::string const & topic) {
                        show_help(description, topic);
                    }
                ),
            "Show help. If given, show help on the specified topic."
        )
        (
            "seed,s",
            po::value<uint64_t>(),
            "Random seed for nonce generation"
        )
        (
            "port,p",
            po::value<uint64_t>()
                ->default_value(3030)
                ->notifier(
                    [](uint64_t const port) {
                        check_port(port);
                    }
                ),
            "TCP port to use for incoming connections"
        )
        (
            "peers,p",
            po::value<std::vector<std::string>>()
                ->multitoken()
                ->composing()
                ->notifier(
                    [](std::vector<std::string> const peers) {
                        for (auto peer: peers) {
                            check_socket(peer);
                        }
                    }
                )
            ,
            "List of peer nodes"
        )
    ;

    po::variables_map args;

    try {
        po::store(
            po::parse_command_line(argc, argv, description),
            args
        );
        po::notify(args);
    } catch (po::error const & e) {
        std::cerr << "Error: " << e.what() << '\n';
        show_help(description, "", EXIT_FAILURE);
    }

    return args;
}

} // namespace naivecoin
