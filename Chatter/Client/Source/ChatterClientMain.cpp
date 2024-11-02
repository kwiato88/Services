#include <iostream>
#include <ChatterClientApp.hpp>
#include <SockSocketUtils.hpp>
#include "ChatterClientDialog.hpp"

WinApi::InstanceHandle hModule;

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
            WinApi::ChatterClientDialog dlg(hModule, WinApi::Handle(0), name);
            dlg.show();
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
