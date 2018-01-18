#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <simple-web-server/client_http.hpp>

#include <spdlog/spdlog.h>

#include <naivecoin/core/block.h>
#include <naivecoin/core/serialize.h>

#include <naivecoin/transaction/transaction.h>
#include <naivecoin/transaction/serialize.h>

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
        client.request("GET", "/query/blockchain", [&logger](auto response, auto error_code) {
            if (error_code) {
                logger->error("Error: {}", error_code.message());
            } else {
                std::string const data = response->content.string();
                std::list<naivecoin::core::Block> const blockchain = naivecoin::core::deserialize_blockchain(data);
                for (naivecoin::core::Block block: blockchain) {
                    std::list<naivecoin::transaction::Transaction> const transactions = naivecoin::transaction::deserialize_transactions(block.data);
                    for (auto transaction: transactions) {
                        std::cout << transaction.id << ", " << transaction.outputs.front() << '\n';
                    }
                }
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
