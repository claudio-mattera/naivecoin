#include <cstdlib>
#include <iostream>
#include <thread>

#include <boost/asio.hpp>

#include "controlserver.h"
#include "miner.h"


void start_miner(naivecoin::Miner & miner)
{
    miner.start();
}

int main()
{
    naivecoin::Miner miner;
    try
    {
        std::thread miner_thread = std::thread(start_miner, std::ref(miner));

        boost::asio::io_service io_service;
        naivecoin::control_server server(io_service, miner);
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
