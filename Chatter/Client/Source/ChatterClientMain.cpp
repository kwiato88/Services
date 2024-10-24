#include <iostream>
#include <ChatterClientApp.hpp>
#include <SockSocketUtils.hpp>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Chatter: Usage: " << argv[0] << " <clientName>" << std::endl;
        return 1;
    }
    try
    {
        sock::init();
        {
            std::string name{argv[1]};
            std::cout << "Chatter: start client " << name << std::endl;
            Chatter::ClientApp app(name);
            app.run();
            std::cout << "Chatter: client " << name << " finished" << std::endl;
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
