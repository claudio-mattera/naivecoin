#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <boost/asio.hpp>

#include <spdlog/spdlog.h>

#include <naivecoin/core/block.h>
#include <naivecoin/core/serialize.h>

#include <naivecoin/transaction/transaction.h>
#include <naivecoin/transaction/serialize.h>

#include "options.h"

void initialize_loggers()
{
    spdlog::stdout_logger_mt("main");
}

std::string read_file(std::string const & filename)
{
    std::ifstream stream(filename);
    return std::string(
        std::istreambuf_iterator<char>(stream),
        std::istreambuf_iterator<char>()
    );
}

std::string make_request(std::string const & path, std::string const & data)
{
    std::ostringstream stream;
    stream << "GET " << path << " HTTP/1.0\r\n";
    if (! data.empty()) {
        stream << "Content-Length: " << data.size() << "\r\n";
    }
    stream << "\r\n" << data;
    return stream.str();
}

std::string get_response_data(std::string const & response)
{
    std::size_t data_index = response.find("\r\n\r\n");
    return response.substr(data_index + 4);
}

int main(int argc, char * argv[])
{
    auto const options = naivecoin::process_program_options(argc, argv);

    initialize_loggers();
    auto logger = spdlog::get("main");

    auto const public_key = read_file(options["public"].as<std::string>());
    auto const private_key = read_file(options["private"].as<std::string>());
    auto const node = options["node"].as<std::string>();

    try {
        boost::asio::io_service io_service;

        boost::asio::ip::tcp::resolver resolver(io_service);
        std::size_t const colon_index = node.find(":");
        boost::asio::ip::tcp::resolver::query query(
            node.substr(0, colon_index),
            node.substr(colon_index + 1)
        );
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        boost::asio::ip::tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        std::string const request = make_request("/query/blockchain", "");

        boost::system::error_code ignored_error;
        boost::asio::write(socket, boost::asio::buffer(request), ignored_error);

        std::cout << ignored_error << '\n';

        std::ostringstream stream;

        for (;;)
        {
            boost::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = socket.read_some(boost::asio::buffer(buf), error);

            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.

            stream.write(buf.data(), len);
        }

        std::string const response = stream.str();
        std::string const response_data = get_response_data(response);

        std::list<naivecoin::Block> const blockchain = naivecoin::deserialize_blockchain(response_data);
        for (naivecoin::Block block: blockchain) {
            std::list<naivecoin::Transaction> const transactions = naivecoin::deserialize_transactions(block.data);
            for (auto transaction: transactions) {
                std::cout << transaction.id << '\n';
            }
        }
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
