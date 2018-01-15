#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <thread>
#include <functional>
#include <future>

#include <spdlog/spdlog.h>

#include "node.h"
#include "options.h"

void initialize_loggers()
{
    spdlog::stdout_logger_mt("main");
    spdlog::stdout_logger_mt("node");
    spdlog::stdout_logger_mt("miner");
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

    uint64_t const port = options["port"].as<uint64_t>();
    naivecoin::Node node(port, public_key, peers, seed);

    logger->info("Launching node thread");
    auto main_future = std::async(std::launch::async, [&node](){node.start();});

    return EXIT_SUCCESS;
}
