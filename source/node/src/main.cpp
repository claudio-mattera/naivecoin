#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <thread>
#include <functional>
#include <future>

#include <boost/asio.hpp>

#include <spdlog/spdlog.h>

#include "dataserver.h"
#include "node.h"
#include "options.h"

void initialize_loggers()
{
    spdlog::stdout_logger_mt("main");
    spdlog::stdout_logger_mt("node");
    spdlog::stdout_logger_mt("miner");
    spdlog::stdout_logger_mt("dataserver");
    spdlog::stdout_logger_mt("dataconnection");
    spdlog::stdout_logger_mt("controlserver");
    spdlog::stdout_logger_mt("controlconnection");
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
    auto const options = naivecoin::process_program_options(argc, argv);

    initialize_loggers();
    auto logger = spdlog::get("main");

    uint64_t const seed = options.count("seed")
        ? options["seed"].as<uint64_t>()
        : std::time(nullptr);

    auto const peers = options.count("peers")
        ? options["peers"].as<std::vector<std::string>>()
        : std::vector<std::string>();

    auto const public_key = read_file(options["public"].as<std::string>());

    naivecoin::Node node(public_key, peers, seed);

    uint64_t const data_port = options["port"].as<uint64_t>();
    try
    {
        logger->info("Launching node thread");
        auto main_future = std::async(std::launch::async, [&node](){node.start();});

        boost::asio::io_service io_service;

        naivecoin::MessageHandler const message_handler = std::bind(
            & naivecoin::Node::process_message,
            & node,
            std::placeholders::_1,
            std::placeholders::_2
        );
        naivecoin::data_server data_server(message_handler, io_service, data_port);
        logger->info("Launching server thread");
        io_service.run();
    }
    catch (std::exception& e)
    {
        auto console = spdlog::stdout_color_mt("console");
        console->critical(e.what());
    }

    return EXIT_SUCCESS;
}
