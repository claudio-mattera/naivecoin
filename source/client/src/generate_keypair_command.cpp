#include "generate_keypair_command.h"

#include <cstdlib>
#include <fstream>

#include <simple-web-server/client_http.hpp>

#include <spdlog/spdlog.h>

#include <naivecoin/core/utils.h>

#include <naivecoin/crypto/crypto.h>

#include "utils.h"

namespace po = boost::program_options;

int generate_keypair_command(boost::program_options::variables_map const & variables_map)
{
    auto logger = spdlog::get("generate_keypair_command");
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
    return EXIT_SUCCESS;
};

std::pair<
    po::options_description,
    std::function<int(po::variables_map const &)>
> get_generate_keypair_pair()
{
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

    return std::make_pair(generate_keypair_options, generate_keypair_command);
}
