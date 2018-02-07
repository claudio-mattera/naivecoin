#include "query_transactions_command.h"

#include <cstdlib>
#include <list>

#include <simple-web-server/client_http.hpp>

#include <spdlog/spdlog.h>

#include <naivecoin/core/utils.h>

#include <naivecoin/serialization/transaction.h>

#include "utils.h"

namespace po = boost::program_options;

int query_transactions_command(boost::program_options::variables_map const & variables_map)
{
    auto logger = spdlog::get("query_transactions_command");
    auto const public_key = read_file(variables_map["public"].as<std::string>());
    auto const node = variables_map["node"].as<std::string>();

    try {
        logger->debug("Query transactions for address {}", naivecoin::core::replace(public_key, "\n", "_"));
        SimpleWeb::Client<SimpleWeb::HTTP> client(node);
        SimpleWeb::CaseInsensitiveMultimap header;
        header.emplace("Address", naivecoin::core::replace(public_key, "\n", "_"));
        client.request("GET", "/query/transactions", "", header, [&logger, &public_key](auto response, auto error_code) {
            if (error_code) {
                logger->error("Error: {}", error_code.message());
            } else {
                std::string const data = response->content.string();
                std::list<naivecoin::transaction::Transaction> const transactions = naivecoin::serialization::deserialize_transactions(data);
                uint64_t running_total = 0ul;
                for (auto transaction: transactions) {
                    uint64_t total_output = 0ul;
                    uint64_t total_input = 0ul;
                    for (auto output: transaction.outputs) {
                        if (output.address == public_key) {
                            total_output += output.amount;
                        }
                    }
                    for (auto input: transaction.inputs) {
                        for (auto other_transaction: transactions) {
                            if (other_transaction.id == input.transaction_output_id) {
                                for (auto output: other_transaction.outputs) {
                                    if (output.address == public_key) {
                                        total_input += output.amount;
                                    }
                                }
                            }
                        }
                    }

                    running_total += total_output - total_input;

                    int64_t difference = total_output - total_input;

                    logger->info("{:>+10d}, balance: {:>10d} (Id: {})", difference, running_total, transaction.id);
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

std::pair<
    po::options_description,
    std::function<int(po::variables_map const &)>
> get_query_transactions_pair()
{
    po::options_description query_transactions_options("Usage");
    query_transactions_options.add_options()
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

    return std::make_pair(query_transactions_options, query_transactions_command);
}
