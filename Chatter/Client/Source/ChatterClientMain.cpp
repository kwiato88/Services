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

class App
{
public:
    ~App()
    {
        logOut();
    }
    void registerUser()
    {
        //TODO: query for user name and password
        std::string password;
        std::cout << "Chatter: register " << user << std::endl;
        if(!Chatter::ClientApp::registerAtServer(user, password))
        {
            WinApi::MessageDialog{WinApi::Handle(0)}.withTitle("Register")
                .withContent("Failed to register " + user).show();
            throw std::runtime_error("Failed to register " + user);
        }
        WinApi::MessageDialog{WinApi::Handle(0)}.withTitle("Register")
            .withContent("Registered " + user).show();
    }
    void runClient()
    {
        logIn();
        std::cout << "Chatter: start client " << user << std::endl;
        WinApi::ChatterClientDialog dlg(hModule, WinApi::Handle(0), user, cookie);
        dlg.show();
        std::cout << "Chatter: client " << user << " finished" << std::endl;
    }
    void logIn()
    {
        //TODO: query for user name and password
        std::string password;
        std::cout << "Chatter: login " << user << std::endl;
        cookie = Chatter::ClientApp::logIn(user, password);
        if(cookie.empty())
        {
            WinApi::MessageDialog{WinApi::Handle(0)}.withTitle("Login")
                .withContent("Failed to login " + user).show();
            throw std::runtime_error("Failed to login " + user);
        }
    }
    void logOut()
    {
        if(!cookie.empty())
        {
            Chatter::ClientApp::logOut(cookie);
        }
    }
private:
    std::string user;
    std::string cookie;
};

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        std::cerr << "Chatter: Usage: " << argv[0] << " (register)" << std::endl;
        return 1;
    }
    SockEnv sockEnv;
    try
    {
        if (argc == 2 && argv[1] == "register")
        {
            App client;
            client.registerUser();
            return 0;
        }
        App client;
        client.runClient();
        return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "Chatter: " << e.what() << std::endl;
        return 1;
    }
}
