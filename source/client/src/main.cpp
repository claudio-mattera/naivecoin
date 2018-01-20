#include <cstdlib>

#include <fstream>
#include <regex>
#include <vector>

#include <boost/program_options.hpp>

#include <simple-web-server/client_http.hpp>

#include <spdlog/spdlog.h>

#include <naivecoin/core/utils.h>
#include <naivecoin/crypto/crypto.h>

#include "commandlinehandler.h"

namespace po = boost::program_options;

namespace {

void initialize_loggers()
{
    spdlog::stdout_logger_mt("main");
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

} // unnamed namespace

int main(int argc, char * argv[])
{
    initialize_loggers();
    auto logger = spdlog::get("main");

    std::vector<std::string> const command_line(argv + 1, argv + argc);

    po::options_description generate_keypair_options("Usage");
    generate_keypair_options.add_options()
        (
            "public",
            po::value<std::string>()
                ->required()
            ,
            "Public key filename"
        )
        (
            "private",
            po::value<std::string>()
                ->required()
            ,
            "Private key filename"
        )
    ;
    auto generate_keypair_command = [&logger](boost::program_options::variables_map const & variables_map) {
        logger->info("Generating new key pair in {}", variables_map["public"].as<std::string>());
        std::pair<std::string, std::string> const key_pair = naivecoin::crypto::generate_key_pair();
        {
            std::ofstream public_key_file(variables_map["public"].as<std::string>());
            public_key_file << key_pair.first;
        }
        {
            std::ofstream private_key_file(variables_map["private"].as<std::string>());
            private_key_file << key_pair.second;
        }
    };

    po::options_description query_balance_options("Usage");
    query_balance_options.add_options()
        (
            "node",
            po::value<std::string>()
                ->required()
                ->notifier(
                    [](std::string const node) {
                        check_socket(node);
                    }
                )
            ,
            "List of peer nodes"
        )
        (
            "public",
            po::value<std::string>()
                ->required()
            ,
            "Public key filename"
        )
    ;
    auto query_balance_command = [&logger](boost::program_options::variables_map const & variables_map) {
        auto const public_key = read_file(variables_map["public"].as<std::string>());
        auto const node = variables_map["node"].as<std::string>();

        try {
            SimpleWeb::Client<SimpleWeb::HTTP> client(node);
            SimpleWeb::CaseInsensitiveMultimap header;
            header.emplace("Address", naivecoin::core::replace(public_key, "\n", "_"));
            client.request("GET", "/query/balance", "", header, [&logger](auto response, auto error_code) {
                if (error_code) {
                    logger->error("Error: {}", error_code.message());
                } else {
                    std::string const data = response->content.string();
                    logger->info("Balance: {}", data);
                }
            });
            client.io_service->run();
        }
        catch (std::exception& e)
        {
          std::cerr << e.what() << std::endl;
        }
    };

    std::map<std::string, naivecoin::client::Handler> handlers;
    handlers["generate-keypair"] = naivecoin::client::create_handler(
        generate_keypair_options,
        generate_keypair_command
    );
    handlers["balance"] = naivecoin::client::create_handler(
        query_balance_options,
        query_balance_command
    );

    naivecoin::client::handle_commands(command_line, handlers);

    return EXIT_SUCCESS;
}
