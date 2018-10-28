#include <cstdlib>

#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>

#include "commandlinehandler.h"
#include "utils.h"
#include "generate_keypair_command.h"
#include "query_balance_command.h"
#include "query_transactions_command.h"
#include "submit_transaction_command.h"

namespace {

void initialize_loggers()
{
    spdlog::stdout_logger_mt("main");
    spdlog::stdout_logger_mt("commandlinehandler");
    spdlog::stdout_logger_mt("query_balance_command");
    spdlog::stdout_logger_mt("submit_transaction_command");
    spdlog::stdout_logger_mt("query_transactions_command");
    spdlog::stdout_logger_mt("generate_keypair_command");

    spdlog::set_pattern("%v");
}

} // unnamed namespace

int main(int argc, char * argv[])
{
    initialize_loggers();
    auto logger = spdlog::get("main");

    std::vector<std::string> const command_line(argv + 1, argv + argc);

    std::map<std::string, naivecoin::client::Handler> handlers;

    auto generate_keypair_pair = get_generate_keypair_pair();
    handlers["generate-keypair"] = naivecoin::client::create_handler(
        generate_keypair_pair.first,
        generate_keypair_pair.second
    );
    auto query_balance_pair = get_query_balance_pair();
    handlers["balance"] = naivecoin::client::create_handler(
        query_balance_pair.first,
        query_balance_pair.second
    );
    auto query_transactions_pair = get_query_transactions_pair();
    handlers["transactions"] = naivecoin::client::create_handler(
        query_transactions_pair.first,
        query_transactions_pair.second
    );
    auto submit_transaction_pair = get_submit_transaction_pair();
    handlers["submit-transaction"] = naivecoin::client::create_handler(
        submit_transaction_pair.first,
        submit_transaction_pair.second
    );

    return naivecoin::client::handle_commands(command_line, handlers);
}
