#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <thread>
#include <functional>
#include <future>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "node.h"
#include "options.h"

void initialize_loggers()
{
    spdlog::stdout_color_mt("main");
    spdlog::stdout_color_mt("node");
    spdlog::stdout_color_mt("sender");
    spdlog::stdout_color_mt("miner");
    spdlog::stdout_color_mt("peersmanager");

    spdlog::set_pattern("[%Y-%m-%dT%H:%M:%S.%f] [%t] [%n] [%l] %v");
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
    auto const options = naivecoin::node::process_program_options(argc, argv);

    initialize_loggers();
    auto logger = spdlog::get("main");

    uint64_t const seed = options.count("seed")
        ? options["seed"].as<uint64_t>()
        : std::time(nullptr);

    auto const peers = options.count("peers")
        ? options["peers"].as<std::vector<std::string>>()
        : std::vector<std::string>();

    auto const public_key = read_file(options["public"].as<std::string>());

    uint64_t const port = options["port"].as<uint64_t>();
    uint64_t const sleep_time = options["sleep"].as<uint64_t>();
    naivecoin::node::Node node(port, public_key, peers, sleep_time, seed);

    logger->info("Launching node thread");
    auto main_future = std::async(std::launch::async, [&node](){node.start();});

    return EXIT_SUCCESS;
}
