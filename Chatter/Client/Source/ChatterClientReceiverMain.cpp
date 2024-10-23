#include <iostream>
#include "ChatterClientReceiver.hpp"
#include "SockSocketUtils.hpp"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Chatter: Usage: " << argv[0] << " <host> <port>" << std::endl;
        return 1;
    }
    try
    {
        sock::init();
        {
            std::string host = argv[1];
            std::string port = argv[2];
            std::cout << "Chatter: setup client receiver at [" << host << ":" << port << "]" << std::endl;
            Chatter::ClientReceiver receiver(host, port);
            receiver.start();
            std::cout << "Chatter: client receiver finished" << std::endl;
        }
        sock::cleanup();
        return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "Chatter: " << e.what() << std::endl;
        sock::cleanup();
        return 1;
    }
}
