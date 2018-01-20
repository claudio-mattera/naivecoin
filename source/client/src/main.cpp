#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <iterator>

#include <simple-web-server/client_http.hpp>

#include <spdlog/spdlog.h>

#include <naivecoin/core/block.h>
#include <naivecoin/core/utils.h>

#include <naivecoin/transaction/transaction.h>

#include "options.h"

void initialize_loggers()
{
    spdlog::stdout_logger_mt("main");
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

int main(int argc, char * argv[])
{
    auto const options = naivecoin::client::process_program_options(argc, argv);

    initialize_loggers();
    auto logger = spdlog::get("main");

    auto const public_key = read_file(options["public"].as<std::string>());
    auto const private_key = read_file(options["private"].as<std::string>());
    auto const node = options["node"].as<std::string>();

    try {
        SimpleWeb::Client<SimpleWeb::HTTP> client(node);
        SimpleWeb::CaseInsensitiveMultimap header;
        header.emplace("Address", naivecoin::core::replace(public_key, "\n", "_"));
        client.request("GET", "/query/balance", "", header, [&logger](auto response, auto error_code) {
            if (error_code) {
                logger->error("Error: {}", error_code.message());
            } else {
                std::string const data = response->content.string();
                std::cout << "Balance: " << data << '\n';
            }
        });
        client.io_service->run();
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
