#include <iostream>
#include "ChatterService.hpp"
#include "ServiceProviderClient.hpp"
#include "MsgTcpIpServer.hpp"
#include "MsgTcpIpConnection.hpp"

namespace Chatter
{

class AllowAll : public IAuthenticator
{
public:
    bool addUser(const std::string&, const std::string&) override
    {
        return true;
    }
    void removeUser(const std::string&) override
    {
    }
    bool authenticate(const std::string&, const std::string&) override
    {
        return true;
    }
    std::list<std::string> activeUsers()
    {
        return {};
    }
};

Service::Service()
    : BaseService(std::bind(&Service::createServer, this)),
      chatter(std::make_shared<Server>(
        [](const auto& p_host, const auto& p_port) { return std::make_unique<msg::TcpIpConnection>(p_host, p_port); },
        std::make_unique<AllowAll>()))
{
    setup();
    add<Msg::Register, Msg::Result>();
    add<Msg::UnRegister>();
    add<Msg::OnLine, Msg::Result>();
    add<Msg::OffLine>();
    add<Msg::Message, Msg::MessageAck>();
    addHandler<Msg::Stop>(Msg::Stop::id, std::make_shared<StopHandler<Msg::Stop> >(*this));
}

void Service::setup()
{
    Networking::ServiceProviderClient provider;
    auto addr = provider.setServiceAddr(name);
    host = addr.host;
    port = addr.port;
    std::cout << "Chatter: setup service at [" << host << ":" << port << "]" << std::endl;
}

Service::~Service()
{
    try
    {
        Networking::ServiceProviderClient provider;
        provider.removeServiceAddr(name);
        std::cout << "Chatter: release addr" << std::endl;
    }
    catch (std::exception&)
    {
    }
}

std::unique_ptr<msg::Server> Service::createServer()
{
    return std::make_unique<msg::TcpIpServer>(host, port);
}

}
