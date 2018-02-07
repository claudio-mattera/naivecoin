#include "submit_transaction_command.h"

#include <cstdlib>
#include <list>

#include <simple-web-server/client_http.hpp>

#include <spdlog/spdlog.h>

#include <naivecoin/core/utils.h>

#include <naivecoin/serialization/unspent_output.h>
#include <naivecoin/serialization/transaction.h>

#include "utils.h"

namespace po = boost::program_options;

int submit_transaction_command(po::variables_map const & variables_map)
{
    auto logger = spdlog::get("submit_transaction_command");
    auto const destination_public_key = read_file(variables_map["destination"].as<std::string>());
    auto const origin_public_key = read_file(variables_map["origin-public"].as<std::string>());
    auto const origin_private_key = read_file(variables_map["origin-private"].as<std::string>());
    uint64_t const amount = variables_map["amount"].as<uint64_t>();
    auto const node = variables_map["node"].as<std::string>();

    std::list<naivecoin::transaction::UnspentOutput> unspent_outputs;

    try {
        SimpleWeb::Client<SimpleWeb::HTTP> client(node);
        SimpleWeb::CaseInsensitiveMultimap header;
        header.emplace("Address", naivecoin::core::replace(origin_public_key, "\n", "_"));
        client.request("GET", "/query/unspent-outputs", "", header, [&logger, &unspent_outputs](auto response, auto error_code) {
            if (error_code) {
                logger->error("Error: {}", error_code.message());
            } else {
                std::string const data = response->content.string();
                unspent_outputs = naivecoin::serialization::deserialize_unspent_outputs(data);
                logger->info("Retrieved {} unspent outputs", unspent_outputs.size());
            }
        });
        client.io_service->run();
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }


    uint64_t amount_found = 0;

    std::list<naivecoin::transaction::Input> inputs;
    std::list<std::string> private_keys;

    std::list<naivecoin::transaction::Output> outputs{
        {destination_public_key, amount}
    };

    for (auto unspent_output: unspent_outputs) {
        if (unspent_output.address != origin_public_key) {
            continue;
        }

        uint64_t const amount_left = amount - amount_found;
        uint64_t const taken_share = std::min(amount_left, unspent_output.amount);
        uint64_t const left_share = unspent_output.amount - taken_share;
        amount_found += taken_share;

        inputs.push_back(
            {unspent_output.transaction_id, unspent_output.transaction_index}
        );
        private_keys.push_back(origin_private_key);

        if (amount_found == amount) {
            if (left_share > 0) {
                outputs.push_back(
                    {origin_public_key, left_share}
                );
            }

            break;
        }
    }

    if (amount_found < amount) {
        logger->error("Insufficient funds!");
        return EXIT_FAILURE;
    }

    naivecoin::transaction::Transaction const transaction = naivecoin::transaction::create_transaction(
        inputs,
        outputs,
        private_keys,
        unspent_outputs
    );

    std::string const serialized_transaction = naivecoin::serialization::serialize_transaction(transaction);

    try {
        SimpleWeb::Client<SimpleWeb::HTTP> client(node);
        SimpleWeb::CaseInsensitiveMultimap header;
        logger->info("Submitting transaction");
        client.request("POST", "/submit/transaction", serialized_transaction, header, [&logger](auto /*response*/, auto error_code) {
            if (error_code) {
                logger->error("Error: {}", error_code.message());
            } else {
                logger->info("Transaction submitted");
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
> get_submit_transaction_pair()
{
    po::options_description submit_transaction_options("Usage");
    submit_transaction_options.add_options()
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
            "Node"
        )
        (
            "origin-private",
            po::value<std::string>()
                ->required()
            ,
            "Private key filename"
        )
        (
            "origin-public",
            po::value<std::string>()
                ->required()
            ,
            "Public key filename"
        )
        (
            "destination",
            po::value<std::string>()
                ->required()
            ,
            "Public key filename"
        )
        (
            "amount",
            po::value<uint64_t>()
                ->required()
            ,
            "Amount to transfer"
        )
    ;

    return std::make_pair(submit_transaction_options, submit_transaction_command);
}
