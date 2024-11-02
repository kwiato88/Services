#include <iostream>
#include <functional>
#include <map>
#include <tuple>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include <sstream>
#include "MsgTcpIpConnection.hpp"
#include "MsgClient.hpp"
#include "ChatterClientApp.hpp"
#include "ChatterClientReceiver.hpp"


namespace Chatter
{

bool isOk(Msg::MessageAck::Status p_status)
{
    return p_status == Msg::MessageAck::Status::Sent 
       || p_status == Msg::MessageAck::Status::Buffered;
}

//TODO: extract
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
        commands["help"]("");
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

ClientApp::Receiver::Receiver(const std::string& p_name, const std::string& p_cookie,
    const std::string& p_host, const std::string& p_port,
    const std::string& p_serverHost, const std::string& p_serverPort,
    ReceivedMessageCallback p_onMessageReceived)
    : api(msg::Client([=]() { return std::make_unique<msg::TcpIpConnection>(p_host, p_port); })),
      thread([=](){receiverMain();}),
      serverApi(msg::Client([=]() { return std::make_unique<msg::TcpIpConnection>(p_serverHost, p_serverPort); })),
      host(p_host), port(p_port), cookie(p_cookie),
      onMessageReceived(p_onMessageReceived)
{
    thread.start();
    if(!serverApi.sendReq<Msg::OnLine, Msg::Result>(Msg::OnLine{cookie, p_host, p_port}).success)
    {
        stopReceiver();
        throw std::runtime_error("Failed to go on-line");
    }
}

ClientApp::Receiver::~Receiver()
{
    notifyServerOffLine();
    stopReceiver();
}

void ClientApp::Receiver::notifyServerOffLine()
try
{
    serverApi.sendInd<Msg::OffLine>(Msg::OffLine{cookie});
}
catch(std::exception&)
{}

void ClientApp::Receiver::stopReceiver()
try
{
    if(thread.isRunning())
    {
        api.sendInd<Msg::Stop>(Msg::Stop{});
        thread.join();
    }
}
catch(std::exception&)
{
    thread.stop();
}

void ClientApp::Receiver::receiverMain()
{
    ClientReceiver receiver(host, port, onMessageReceived);
    receiver.start();
}

ClientApp::ClientApp(const std::string& p_name, std::function<void()> p_onMessageReceived)
 : name(p_name), receiverServiceName(p_name + "@Chatter"),
   server(msg::Client([]() { return std::make_unique<msg::TcpIpConnection>("host", "port"); })),
   activeChat(0), onMessageReceived(p_onMessageReceived)
{
    serverAddr = addrs.getServiceAddr("ChatterService");
    server = Client{msg::Client([=]() { return std::make_unique<msg::TcpIpConnection>(serverAddr.host, serverAddr.port); })};
    registerAtServer();
}

void ClientApp::registerAtServer()
{
    cookie = server.sendReq<Msg::Register, Msg::Cookie>(Msg::Register{name}).cookie;
    if(cookie.empty())
    {
        throw std::runtime_error("Failed to register with ChatterServer");
    }
}

std::string ClientApp::goOnLine()
{
    auto receiverAddr = addrs.setServiceAddr(receiverServiceName);
    receiver = std::make_unique<Receiver>(name, cookie, receiverAddr.host, receiverAddr.port,
        serverAddr.host, serverAddr.port,
        [=](const Msg::Message& p_message){ receivedMessage(p_message); });
    return receiverAddr.host + ":" + receiverAddr.port;
}

void ClientApp::unregisterAtServer()
try
{
    server.sendInd<Msg::UnRegister>(Msg::UnRegister{cookie});
}
catch(std::exception&)
{}

void ClientApp::goOffLine()
{
    if(receiver)
    {
        receiver.reset();
        addrs.removeServiceAddr(receiverServiceName);
    }
}

ClientApp::~ClientApp()
{
    goOffLine();
    unregisterAtServer();
}

ClientApp::Chat::Chat(const std::string& p_withUser)
 : withUser(p_withUser)
{}

std::string ClientApp::Chat::toString() const
{
    return std::accumulate(messages.begin(), messages.end(), std::string());
}
void ClientApp::Chat::add(const std::string& p_user, const std::string& p_message)
{
    messages.push_back(std::string("=== ") + p_user + " ===\r\n" + p_message + "\r\n");
}
std::string ClientApp::Chat::with() const
{
    return withUser;
}

void ClientApp::sendMessage(const std::string& p_message)
{
    if(chats.empty())
        return;
    chats.at(activeChat).add(name, p_message);
    sendToServer(p_message);
}

void ClientApp::sendToServer(const std::string& p_message)
{
    Msg::Message msg;
    msg.from = cookie;
    msg.to = chats.at(activeChat).with();
    msg.message = p_message;
    auto status = server.sendReq<Msg::Message, Msg::MessageAck>(msg).status;
    if(!isOk(status))
    {
        std::stringstream ss;
        ss << "Send failed with status " << status;
        throw std::runtime_error(ss.str());
    }
}
std::vector<std::string> ClientApp::getChats() const
{
    std::vector<std::string> names{chats.size(), ""};
    std::transform(chats.begin(), chats.end(), names.begin(), [](const auto& c) { return c.with(); });
    return names;
}
std::string ClientApp::getChatWith(std::size_t p_idx) const
{
    if(p_idx >= chats.size())
        return "";
    return chats.at(p_idx).with();
}
std::string ClientApp::getCurrentChat() const
{
    if(chats.empty())
        return "";
    return chats.at(activeChat).toString();
}
std::size_t ClientApp::getCurrentChatIdx() const
{
    return activeChat;
}
void ClientApp::chatWith(std::size_t p_idx)
{
    if(p_idx < chats.size())
        activeChat = p_idx; 
}
std::size_t ClientApp::newChat(const std::string& p_name)
{
    std::cout << "new :" << p_name << std::endl;
    for(const auto& c : chats)
        std::cout << c.with() << "|";
    std::cout << std::endl;
    auto found = findChatWith(p_name);
    if(found != chats.size())
    {
        for(const auto& c : chats)
            std::cout << c.with() << "|";
        std::cout << std::endl;
        return found;
    }
    chats.push_back(Chat{p_name});
    for(const auto& c : chats)
        std::cout << c.with() << "|";
    std::cout << std::endl;
    return chats.size() - 1;
}
void ClientApp::removeChat(std::size_t p_idx)
{
    std::cout << "remove :" << p_idx << std::endl;
    for(const auto& c : chats)
        std::cout << c.with() << "|";
    std::cout << std::endl;
    if(p_idx >= chats.size())
        return;
    auto activeChatName = chats.at(activeChat).with();
    chats.erase(chats.begin() + p_idx);
    activeChat = findChatWith(activeChatName);
    if(activeChat == chats.size())
        activeChat = 0;
    for(const auto& c : chats)
        std::cout << c.with() << "|";
    std::cout << std::endl;
    std::cout << "active " << activeChat << std::endl;
}
std::size_t ClientApp::findChatWith(const std::string& p_name) const
{
    for(std::size_t i = 0; i < chats.size(); ++i)
        if(chats[i].with() == p_name)
            return i;
    return chats.size();
}
void ClientApp::receivedMessage(const Msg::Message& p_message)
{
    //TODO: mutex
    auto chat = newChat(p_message.from);
    chats.at(chat).add(p_message.from, p_message.message);
    onMessageReceived();
    //TODO: remove
    std::cout << "=== " << p_message.from << " ===\n" << p_message.message << std::endl;
}

}
