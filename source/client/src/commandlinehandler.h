
#pragma once

#ifndef NAIVECOIN_CLIENT_COMMANDLINEHANDLER_H
#define NAIVECOIN_CLIENT_COMMANDLINEHANDLER_H

#include <functional>
#include <string>
#include <vector>
#include <map>

#include <boost/program_options.hpp>

namespace naivecoin::client {

typedef std::function<void(std::vector<std::string> const &)> Handler;

void handle_commands(
    std::vector<std::string> const & command_line,
    std::map<std::string, Handler> const & handlers
);

Handler create_handler(
    boost::program_options::options_description const & description,
    std::function<void(boost::program_options::variables_map const &)> const & command
);

} // namespace naivecoin::client

#endif // NAIVECOIN_CLIENT_COMMANDLINEHANDLER_H
