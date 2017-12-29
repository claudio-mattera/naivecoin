#include <cstdlib>
#include <iostream>
#include <thread>

#include <boost/asio.hpp>

#include "controlserver.h"
#include "miner.h"


void start_miner()
{
    naivecoin::Miner miner;
    miner.start();
}

int main()
{
    try
    {
        std::thread miner_thread(start_miner);

        boost::asio::io_service io_service;
        naivecoin::control_server server(io_service);
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
