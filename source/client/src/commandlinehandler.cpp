#include "commandlinehandler.h"

#include <cstdlib>

#include <spdlog/spdlog.h>

#include <iostream>

namespace po = boost::program_options;

namespace {

void show_help(
    po::options_description const & description,
    std::string const & topic = "",
    int const error_code = EXIT_SUCCESS
)
{
    std::cout << description << '\n';
    if (topic != "") {
        std::cout << "Unknown topic " << topic << '\n';
    }
    std::exit(error_code);
}

}

namespace naivecoin::client {

int handle_commands(
    std::vector<std::string> const & command_line,
    std::map<std::string, Handler> const & handlers
)
{
    auto logger = spdlog::get("commandlinehandler");

    logger->debug("Handlers");
    for (auto h: handlers) {
        logger->debug("{} -> {}", h.first, reinterpret_cast<uint64_t>(& h.second));
    }

    if (command_line.empty()) {
        std::cout << "Missing command" << '\n';
        std::cout << "Available commands: " << '\n';
        for (auto pair: handlers) {
            std::cout << "  * " << pair.first << '\n';
        }
    } else {
        std::string const command_name = command_line.front();
        logger->debug("Command: {}", command_name);
        std::vector<std::string> const rest_of_command_line(
            1 + std::begin(command_line),
            std::end(command_line)
        );

        try {
            auto command = handlers.at(command_name);
            return command(rest_of_command_line);
        } catch (std::out_of_range const &) {
            std::cout << "Unknown command \"" << command_name << "\"" << '\n';
            std::cout << "Available commands: " << '\n';
            for (auto pair: handlers) {
                std::cout << "  * " << pair.first << '\n';
            }
        }
    }
    return EXIT_FAILURE;
}

Handler create_handler(
    boost::program_options::options_description const description,
    std::function<int(boost::program_options::variables_map const &)> const command
)
{
    return [description, command](std::vector<std::string> const & command_line) {
        try {
            po::command_line_parser parser(command_line);

            parser.options(description);

            po::parsed_options const parsed_options = parser.run();
            po::variables_map variables_map;
            po::store(parsed_options, variables_map);
            po::notify(variables_map);

            return command(variables_map);

        } catch (po::error const & e) {
            std::cerr << "Error: " << e.what() << '\n';
            show_help(description, "", EXIT_FAILURE);
            return EXIT_FAILURE;
        }
    };
}

} // namespace naivecoin::client
