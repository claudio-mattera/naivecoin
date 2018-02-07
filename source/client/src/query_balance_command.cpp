#include "query_balance_command.h"

#include <cstdlib>

#include <simple-web-server/client_http.hpp>

#include <spdlog/spdlog.h>

#include <naivecoin/core/utils.h>

#include "utils.h"

namespace po = boost::program_options;

int query_balance_command(po::variables_map const & variables_map)
{
    auto logger = spdlog::get("query_balance_command");
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
    return EXIT_SUCCESS;
};

std::pair<
    po::options_description,
    std::function<int(po::variables_map const &)>
> get_query_balance_pair()
{
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

    return std::make_pair(query_balance_options, query_balance_command);
}
