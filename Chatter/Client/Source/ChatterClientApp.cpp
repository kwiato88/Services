#include <iostream>
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
try
{
    server.sendInd<Msg::OffLine>(Msg::OffLine{cookie});
    std::cout << name << " off-line" << std::endl;
}
catch(std::exception&)
{}

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
    stopReceiver();
    unregisterAtServer();
    addrs.removeServiceAddr(receiverServiceName);
}

void ClientApp::run()
{
    std::cout << "Type <to>:<message> to send a message" << std::endl;
    std::cout << "Type exit to quit" << std::endl;
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
        if(line == "help")
        {
            std::cout << "Type <to>:<message> to send a message" << std::endl;
            std::cout << "Type exit to quit" << std::endl;
            continue;
        }
        auto message = prepareMessage(line);
        if(!message.message.empty() && !message.to.empty())
        {
            sendToReceiver(message);
            sendToServer(message);
        }
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
