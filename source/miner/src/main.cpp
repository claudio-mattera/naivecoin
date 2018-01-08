#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>

#include <boost/asio.hpp>

#include "controlserver.h"
#include "miner.h"
#include "options.h"


void start_miner(naivecoin::Miner & miner)
{
    miner.start();
}

int main(int argc, char * argv[])
{
    auto const options = naivecoin::process_program_options(argc, argv);

    uint64_t const seed = options.count("seed")
        ? options["seed"].as<uint64_t>()
        : std::time(nullptr);

    auto const peers = options.count("peers")
        ? options["peers"].as<std::vector<std::string>>()
        : std::vector<std::string>();

    naivecoin::Miner miner(seed);
    try
    {
        std::thread miner_thread = std::thread(start_miner, std::ref(miner));

        boost::asio::io_service io_service;
        naivecoin::control_server server(io_service, miner);
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
