#include <algorithm>
#include "ChatterServer.hpp"
#include "ChatterCodec.hpp"
#include "ClientWithCodec.hpp"
#include "MsgTcpIpConnection.hpp"

namespace Chatter
{

Server::User::User(ConnectionFactory p_connections, const std::string& p_name)
    : connections(p_connections), name(p_name), sendAction(&User::bufferMsg)
{
}

bool Server::User::is(const std::string& p_name) const
{
    return name == p_name;
}

std::string Server::User::getName() const
{
    return name;
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
    Networking::ClientWithCodec<Msg::Json::Codec> client(msg::Client([=]() { return connections(host, port); }));
    client.sendInd(p_message);
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

Server::Server(ConnectionFactory p_factory)
    : connections(p_factory)
{
}

bool Server::isLogged(const Cookie& p_cookie) const
{
    return loggedUsers.find(p_cookie) != loggedUsers.end();
}

bool Server::isLogged(const std::string& p_userName) const
{
    auto found = std::find_if(loggedUsers.begin(), loggedUsers.end(), [&p_userName](const auto& p_user) { return p_user.second.is(p_userName); });
    return found != loggedUsers.end();
}

bool Server::isRegisteredNotLogged(const std::string& p_userName) const
{
    return notLoggedUsers.find(p_userName) != notLoggedUsers.end();
}

Msg::Result Server::handle(const Msg::Register& p_msg)
{
    if(isRegisteredNotLogged(p_msg.userName) || isLogged(p_msg.userName))
    {
        return Msg::Result{false};
    }
    notLoggedUsers[p_msg.userName] = User{ connections, p_msg.userName };
    return Msg::Result{true};
}

void Server::handle(const Msg::UnRegister& p_msg)
{
    auto it = loggedUsers.find(Cookie{p_msg.cookie});
    if(it != loggedUsers.end())
    {
        it->second.offline();
        loggedUsers.erase(it);
        cookies.releaseCookie(Cookie{p_msg.cookie});
    }
}

Msg::Cookie Server::handle(const Msg::Login& p_msg)
try
{
    if(isLogged(p_msg.userName))
    {
        return Msg::Cookie{"0"};
    }
    auto user = notLoggedUsers.find(p_msg.userName);
    if(user == notLoggedUsers.end())
    {
        return Msg::Cookie{""};
    }
    auto cookie = cookies.allocateCookie();
    loggedUsers[cookie] = std::move(user->second);
    notLoggedUsers.erase(user);
    return Msg::Cookie{ cookie.toString() };
}
catch(std::exception&)
{
    return Msg::Cookie{""};
}

void Server::handle(const Msg::Logout& p_msg)
{
    auto it = loggedUsers.find(Cookie{p_msg.cookie});
    if(it == loggedUsers.end())
    {
        return;
    }
    it->second.offline();
    notLoggedUsers[it->second.getName()] = std::move(it->second);
    loggedUsers.erase(it);
    cookies.releaseCookie(Cookie{p_msg.cookie});
}

Msg::Result Server::handle(const Msg::OnLine& p_msg)
{
    if(!isLogged(Cookie{p_msg.cookie}))
    {
        return Msg::Result{false};
    }
    auto& user = loggedUsers[Cookie{p_msg.cookie}];
    user.online(p_msg.host, p_msg.port);
    return Msg::Result{true};
}

void Server::handle(const Msg::OffLine& p_msg)
{
    if(!isLogged(Cookie{p_msg.cookie}))
    {
        return;
    }
    auto& user = loggedUsers[Cookie{p_msg.cookie}];
    user.offline();
}

Server::User& Server::getUser(const std::string& p_userName)
{
    auto notlogged = notLoggedUsers.find(p_userName);
    if(notlogged != notLoggedUsers.end())
    {
        return notlogged->second;
    }
    auto logged = std::find_if(loggedUsers.begin(), loggedUsers.end(), [&p_userName](const auto& p_user) { return p_user.second.is(p_userName); });
    if(logged != loggedUsers.end())
    {
        return logged->second;
    }
    throw std::runtime_error("User not found");
}

Msg::MessageAck Server::handle(const Msg::Message& p_msg)
{
    if(!isLogged(Cookie{ p_msg.from }) || (!isRegisteredNotLogged(p_msg.to) && !isLogged(p_msg.to)))
    {
        return Msg::MessageAck{Msg::MessageAck::Status::UnknownUser};
    }
    auto& receiver = getUser(p_msg.to);
    auto& sender = loggedUsers[Cookie{p_msg.from}];
    auto message = p_msg;
    message.from = sender.getName();
    return Msg::MessageAck{ receiver.message(message) };
}

} // namespace Chatter
