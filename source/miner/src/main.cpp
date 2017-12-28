#include <cstdlib>
#include <iostream>

#include <boost/asio.hpp>

#include <boost/log/attributes/named_scope.hpp>

#include "controlserver.h"

int main()
{
    try
    {
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
