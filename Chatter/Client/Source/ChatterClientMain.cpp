#include <iostream>
#include <ChatterClientApp.hpp>
#include <SockSocketUtils.hpp>
#include "ChatterClientDialog.hpp"
#include "MessageDialog.hpp"

WinApi::InstanceHandle hModule;

struct SockEnv
{
    SockEnv()
    {
        sock::init();
    }
    ~SockEnv()
    {
        sock::cleanup();
    }
};

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        std::cerr << "Chatter: Usage: " << argv[0] << " (register)" << std::endl;
        return 1;
    }
    SockEnv sockEnv;
    if (argc == 2 && argv[1] == "register")
    {
        std::cout << "Chatter: register user" << std::endl;
        //TODO: query for user name and password
        std::string user, password;
        if(Chatter::ClientApp::registerAtServer(user, password))
        {
            WinApi::MessageDialog{WinApi::Handle(0)}.withTitle("Register")
                .withContent("Registered " + user).show();
            return 0;
        }
        WinApi::MessageDialog{WinApi::Handle(0)}.withTitle("Register")
            .withContent("Failed to register " + user).show();
        return 1;
    }
    try
    {
        //TODO: query for user name and password
        std::string user, password;
        auto cookie = Chatter::ClientApp::logIn(user, password);
        if(cookie.empty())
        {
            WinApi::MessageDialog{WinApi::Handle(0)}.withTitle("Login")
                .withContent("Failed to login " + user).show();
            return 1;
        }
        std::cout << "Chatter: logged " << user << std::endl;
        std::cout << "Chatter: start client " << user << std::endl;
        WinApi::ChatterClientDialog dlg(hModule, WinApi::Handle(0), user, cookie);
        dlg.show();
        std::cout << "Chatter: client " << user << " finished" << std::endl;
        Chatter::ClientApp::logOut(cookie);
        return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "Chatter: " << e.what() << std::endl;
        return 1;
    }
}
