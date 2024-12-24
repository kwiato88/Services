#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <memory>
#include <csignal>
#include "ChatterService.hpp"
#include "SockSocketUtils.hpp"

std::unique_ptr<Chatter::Service> g_chatter;

void stopServer(int)
{
    std::cout << "Chatter: stop server" << std::endl;
    g_chatter.reset();
}

int main()
{
    try
    {
        sock::init();
        {
            std::signal(SIGINT, stopServer);
            std::cout << "Chatter: create service" << std::endl;
            g_chatter = std::make_unique<Chatter::Service>(std::filesystem::path("."));
            g_chatter->start();
            std::cout << "Chatter: service finished" << std::endl;
            g_chatter.reset();
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
