#include <cstdlib>
#include <ctime>
#include <thread>
#include <functional>

#include <boost/asio.hpp>

#include <spdlog/spdlog.h>

#include "dataserver.h"
#include "node.h"
#include "options.h"

void initialize_loggers()
{
    spdlog::stdout_logger_mt("node");
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

    naivecoin::Node node(peers, seed);

    uint64_t const data_port = options["port"].as<uint64_t>();
    try
    {
        boost::asio::io_service io_service;

        auto const message_handler = std::bind(
            & naivecoin::Node::process_message,
            & node,
            std::placeholders::_1
        );
        naivecoin::data_server data_server(message_handler, io_service, data_port);
        io_service.run();
    }
    catch (std::exception& e)
    {
        auto console = spdlog::stdout_color_mt("console");
        console->critical(e.what());
    }

    return EXIT_SUCCESS;
}
