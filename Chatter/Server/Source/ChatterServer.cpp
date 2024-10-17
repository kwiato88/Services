#include <algorithm>
#include "ChatterServer.hpp"
#include "ChatterCodec.hpp"
#include "ClientWithCodec.hpp"
#include "MsgTcpIpConnection.hpp"

namespace Chatter
{

Server::User::User(const std::string& p_name)
    : name(p_name), sendAction(&User::bufferMsg)
{
}

bool Server::User::is(const std::string& p_name) const
{
    return name == p_name;
}

void Server::User::sendBufferedMessages()
{
    while(!bufferedMessages.empty())
    {
        sendMsg(bufferedMessages.front());
        bufferedMessages.pop();
    }
}

void Server::User::online(const std::string& p_host, const std::string& p_port)
{
    host = p_host;
    port = p_port;
    sendAction = &User::sendMsg;
    sendBufferedMessages();
}

void Server::User::offline()
{
    host.clear();
    port.clear();
    sendAction = &User::bufferMsg;
}

Msg::MessageAck::Status Server::User::sendMsg(const Msg::Message& p_message)
try{
    auto msg = p_message;
    msg.from = name;
    Networking::ClientWithCodec<Msg::Json::Codec> client(msg::Client([=]() { return std::make_unique<msg::TcpIpConnection>(host, port); }));
    client.sendInd(msg);
    return Msg::MessageAck::Status::Sent;
}
catch(std::exception&)
{
    return Msg::MessageAck::Status::Failed;
}

Msg::MessageAck::Status Server::User::bufferMsg(const Msg::Message& p_message)
{
    if(bufferedMessages.size() >= bufferCapacity)
    {
        return Msg::MessageAck::Status::Failed;
    }
    bufferedMessages.push(p_message);
    return Msg::MessageAck::Status::Buffered;
}

Msg::MessageAck::Status Server::User::message(const Msg::Message& p_message)
{
    return (this->*sendAction)(p_message);
}

bool Server::isRegistered(const Cookie& p_cookie) const
{
    return users.find(p_cookie) != users.end();
}

bool Server::isRegistered(const std::string& p_userName) const
{
    auto found = std::find_if(users.begin(), users.end(), [&p_userName](const auto& p_user) { return p_user.second.is(p_userName); });
    return found != users.end();
}

Msg::Cookie Server::handle(const Msg::Register& p_msg)
try
{
    if(isRegistered(p_msg.userName))
    {
        return Msg::Cookie{""};
    }
    auto cookie = cookies.allocateCookie();
    users[cookie] = User{ p_msg.userName };
    return Msg::Cookie{ cookie.toString() };
}
catch(std::exception&)
{
    return Msg::Cookie{""};
}

void Server::handle(const Msg::UnRegister& p_msg)
{
    auto it = users.find(Cookie{p_msg.cookie});
    if(it != users.end())
    {
        users.erase(it);
        cookies.releaseCookie(Cookie{p_msg.cookie});
    }
}

Msg::Result Server::handle(const Msg::OnLine& p_msg)
{
    if(!isRegistered(Cookie{p_msg.cookie}))
    {
        return Msg::Result{false};
    }
    auto& user = users[Cookie{p_msg.cookie}];
    user.online(p_msg.host, p_msg.port);
    return Msg::Result{true};
}

void Server::handle(const Msg::OffLine& p_msg)
{
    if(!isRegistered(Cookie{p_msg.cookie}))
    {
        return;
    }
    auto& user = users[Cookie{p_msg.cookie}];
    user.offline();
}

Server::User& Server::getUser(const std::string& p_userName)
{
    auto found = std::find_if(users.begin(), users.end(), [&p_userName](const auto& p_user) { return p_user.second.is(p_userName); });
    if(found == users.end())
    {
        throw std::runtime_error("User not found");
    }
    return found->second;
}

Msg::MessageAck Server::handle(const Msg::Message& p_msg)
{
    if(!isRegistered(Cookie{ p_msg.from }) || !isRegistered(p_msg.to))
    {
        return Msg::MessageAck{Msg::MessageAck::Status::UnknownUser};
    }
    auto& user = getUser(p_msg.to);
    return Msg::MessageAck{ user.message(p_msg) };
}

} // namespace Chatter
