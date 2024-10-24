#include <iostream>
#include <functional>
#include <map>
#include <tuple>
#include "ChatterClientApp.hpp"
#include "MsgClient.hpp"
#include "MsgTcpIpConnection.hpp"

namespace Chatter
{

bool isOk(Msg::MessageAck::Status p_status)
{
    return p_status == Msg::MessageAck::Status::Sent 
       || p_status == Msg::MessageAck::Status::Buffered;
}

class Menu
{
public:
    using Command = std::function<void(const std::string& p_line)>;
    Menu() : defaultCommand([](const auto&){}), helpMessage("Type exit to quit\nType help for help\n")
    {
        commands["help"] = [this](const std::string&) { std::cout << helpMessage << std::endl; };
    }
    Menu& add(const std::string& p_name, const std::string& p_description, Command p_command)
    {
        helpMessage += p_description + "\n";
        commands[p_name] = p_command;
        return *this;
    }
    Menu& addDefault(const std::string& p_description, Command p_command)
    {
        helpMessage += p_description + "\n";
        defaultCommand = p_command;
        return *this;
    }
    void operator()()
    {
        std::string line;
        while(std::getline(std::cin, line))
        {
            if(line.empty())
            {
                continue;
            }
            if(line == "exit")
            {
                break;
            }
            std::string command, args;
            std::tie(command, args) = parseLine(line);
            auto it = commands.find(command);
            if(it != commands.end())
            {
                it->second(args);
            }
            else
            {
                defaultCommand(line);
            }
        }
    }
private:
    std::tuple<std::string, std::string> parseLine(const std::string& p_line)
    {
        auto separator = p_line.find(' ');
        auto command = p_line.substr(0, separator);
        auto args = separator == std::string::npos ? "" : p_line.substr(separator + 1);
        return {command, args};
    }
    std::map<std::string, Command> commands;
    Command defaultCommand;
    std::string helpMessage;
};

ClientApp::ClientApp(const std::string& p_name)
 : name(p_name), receiverServiceName(p_name + "@Chatter"),
   server(msg::Client([]() { return std::make_unique<msg::TcpIpConnection>("host", "port"); })),
   receiver(msg::Client([]() { return std::make_unique<msg::TcpIpConnection>("host", "port"); }))
{
    auto chatterAddr = addrs.getServiceAddr("ChatterService");
    server = Client{msg::Client([=]() { return std::make_unique<msg::TcpIpConnection>(chatterAddr.host, chatterAddr.port); })};
    registerAtServer();
    goOnLine();
}

void ClientApp::registerAtServer()
{
    cookie = server.sendReq<Msg::Register, Msg::Cookie>(Msg::Register{name}).cookie;
    if(cookie.empty())
    {
        throw std::runtime_error("Failed to register with ChatterServer");
    }
    std::cout << name << " registered" << std::endl;
}

void ClientApp::goOnLine()
{
    auto receiverAddr = addrs.setServiceAddr(receiverServiceName);
    receiver = Client{msg::Client([=]() { return std::make_unique<msg::TcpIpConnection>(receiverAddr.host, receiverAddr.port); })};
    if(!server.sendReq<Msg::OnLine, Msg::Result>(Msg::OnLine{cookie, receiverAddr.host, receiverAddr.port}).success)
    {
        throw std::runtime_error("Failed to go on-line with ChatterServer");
    }
    std::cout << name << " on-line at [" << receiverAddr.host << ":" << receiverAddr.port << "]" << std::endl;
}

void ClientApp::unregisterAtServer()
try
{
    server.sendInd<Msg::UnRegister>(Msg::UnRegister{cookie});
    std::cout << name << " unregistered" << std::endl;
}
catch(std::exception&)
{}

void ClientApp::goOffLine()
{
    try
    {
        server.sendInd<Msg::OffLine>(Msg::OffLine{cookie});
    }
    catch(std::exception&)
    {}
    stopReceiver();
    addrs.removeServiceAddr(receiverServiceName);
    std::cout << name << " off-line" << std::endl;
}

void ClientApp::stopReceiver()
try
{
    receiver.sendInd<Msg::Stop>(Msg::Stop{});
}
catch(std::exception&)
{}

ClientApp::~ClientApp()
{
    goOffLine();
    unregisterAtServer();
}

void ClientApp::run()
{
    Menu menu;
    menu
    .add("online",  "online: Go on-line", [this](const std::string&) { goOnLine(); })
    .add("offline", "offline: Go off-line", [this](const std::string&) { goOffLine(); })
    .addDefault("Type <to>:<message> to send a message", [this](const std::string& p_line){ sendMessage(p_line); });
    menu();
}

void ClientApp::sendMessage(const std::string& p_line)
{
    auto message = prepareMessage(p_line);
    if(!message.message.empty() && !message.to.empty())
    {
        sendToReceiver(message);
        sendToServer(message);
    }
}

Msg::Message ClientApp::prepareMessage(const std::string& p_line)
{
    auto separator = p_line.find(':');
    if(separator == std::string::npos)
    {
        return Msg::Message{};
    }
    auto to = p_line.substr(0, separator);
    auto message = p_line.substr(separator + 1);
    if(to.empty() || message.empty())
    {
        return Msg::Message{};
    }
    Msg::Message msg;
    msg.to = to;
    msg.message = message;
    return msg;
}

void ClientApp::sendToReceiver(Msg::Message p_msg)
{
    p_msg.from = name;
    receiver.sendInd<Msg::Message>(p_msg);
}

void ClientApp::sendToServer(Msg::Message p_msg)
{
    p_msg.from = cookie;
    auto status = server.sendReq<Msg::Message, Msg::MessageAck>(p_msg).status;
    std::cout << (isOk(status) ? "Message sent" : "Failed to send message") <<" with status " << status << std::endl;
}

}
