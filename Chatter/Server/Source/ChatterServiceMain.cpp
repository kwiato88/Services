#include <iostream>
#include <stdexcept>
#include "ChatterService.hpp"
#include "SockSocketUtils.hpp"

int main()
{
    try
    {
        sock::init();
        {
            std::cout << "Chatter: create service" << std::endl;
            Chatter::Service chatter;
            chatter.start();
            std::cout << "Chatter: service finished" << std::endl;
        }
        sock::cleanup();
        return 0;
    }
    catch(std::exception& e)
    {
        std::cerr << "Chatter: exception " << e.what() << '\n';
        sock::cleanup();
        return 1;
    }
}
