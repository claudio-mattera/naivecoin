#include <cstdlib>

#include <fstream>
#include <regex>
#include <vector>

#include <boost/program_options.hpp>

#include <simple-web-server/client_http.hpp>

#include <spdlog/spdlog.h>

#include <naivecoin/core/utils.h>
#include <naivecoin/crypto/crypto.h>
#include <naivecoin/serialization/unspent_output.h>
#include <naivecoin/serialization/transaction.h>
#include <naivecoin/transaction/output.h>
#include <naivecoin/transaction/input.h>
#include <naivecoin/transaction/transaction.h>
#include <naivecoin/transaction/unspent_output.h>

#include "commandlinehandler.h"

namespace po = boost::program_options;

namespace {

void initialize_loggers()
{
    spdlog::stdout_logger_mt("main");
    spdlog::stdout_logger_mt("commandlinehandler");

    spdlog::set_pattern("%v");
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
    auto submit_transaction_command = [&logger](boost::program_options::variables_map const & variables_map) {
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

    };

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
    auto query_transactions_command = [&logger](boost::program_options::variables_map const & variables_map) {
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
    handlers["transactions"] = naivecoin::client::create_handler(
        query_transactions_options,
        query_transactions_command
    );
    handlers["submit-transaction"] = naivecoin::client::create_handler(
        submit_transaction_options,
        submit_transaction_command
    );

    naivecoin::client::handle_commands(command_line, handlers);

    return EXIT_SUCCESS;
}
