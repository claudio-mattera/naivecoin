#include <cstdlib>
#include <ctime>
#include <thread>

#include <boost/asio.hpp>

#include <spdlog/spdlog.h>

#include "controlserver.h"
#include "dataserver.h"
#include "miner.h"
#include "options.h"


void start_miner(naivecoin::Miner & miner)
{
    miner.start();
}

void initialize_loggers()
{
    spdlog::stdout_logger_mt("miner");
    spdlog::stdout_logger_mt("dataserver");
    spdlog::stdout_logger_mt("dataconnection");
    spdlog::stdout_logger_mt("controlserver");
    spdlog::stdout_logger_mt("controlconnection");
}

int main(int argc, char * argv[])
{
    auto const options = naivecoin::process_program_options(argc, argv);

    initialize_loggers();

    uint64_t const seed = options.count("seed")
        ? options["seed"].as<uint64_t>()
        : std::time(nullptr);

    auto const peers = options.count("peers")
        ? options["peers"].as<std::vector<std::string>>()
        : std::vector<std::string>();

    naivecoin::Miner miner(seed);

    uint64_t const control_port = options["control-port"].as<uint64_t>();
    uint64_t const data_port = options["data-port"].as<uint64_t>();
    try
    {
        std::thread miner_thread = std::thread(start_miner, std::ref(miner));

        boost::asio::io_service io_service;
        naivecoin::control_server control_server(io_service, control_port, miner);
        naivecoin::data_server data_server(io_service, data_port, miner);
        io_service.run();
    }
    catch (std::exception& e)
    {
        auto console = spdlog::stdout_color_mt("console");
        console->critical(e.what());
    }

    return EXIT_SUCCESS;
}
