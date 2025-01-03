#include <algorithm>
#include <iostream>
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

Cookie Server::User::getCookie() const
{
    return cookie;
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

void Server::User::login(const Cookie& p_cookie)
{
    cookie = p_cookie;
}

void Server::User::logout()
{
    cookie = Cookie{};
}

bool Server::User::isLogged() const
{
    return cookie != Cookie{};
}

Msg::MessageAck::Status Server::User::sendMsg(const Msg::Message& p_message)
try{
    Networking::ClientWithCodec<Msg::Json::Codec> client(msg::Client([=]() { return connections(host, port); }));
    client.sendInd(p_message);
    return Msg::MessageAck::Status::Sent;
}
catch(std::exception& e)
{
    std::cerr << "Chatter: Failed to send message from " << name << " to " << p_message.to << ". " << e.what() << std::endl;
    return Msg::MessageAck::Status::Failed;
}

Msg::MessageAck::Status Server::User::bufferMsg(const Msg::Message& p_message)
{
    bufferedMessages.push(p_message);
    if(bufferedMessages.size() >= bufferCapacity)
    {
        bufferedMessages.pop();
    }
    return Msg::MessageAck::Status::Buffered;
}

Msg::MessageAck::Status Server::User::message(const Msg::Message& p_message)
{
    return (this->*sendAction)(p_message);
}

Server::Server(ConnectionFactory p_factory, std::unique_ptr<IAuthenticator> p_authenticator)
    : connections(p_factory), authenticator(std::move(p_authenticator))
{
    for(const auto& user : authenticator->activeUsers())
    {
        addUser(user);
    }
}

bool Server::isLogged(const Cookie& p_cookie) const
{
    return loggedUsers.find(p_cookie) != loggedUsers.end();
}

bool Server::isRegistered(const std::string& p_userName) const
{
    return allUsers.find(p_userName) != allUsers.end();
}

Msg::Result Server::handle(const Msg::Register& p_msg)
{
    if(isRegistered(p_msg.userName) || !authenticator->addUser(p_msg.userName, p_msg.password))
    {
        return Msg::Result{false};
    }
    addUser(p_msg.userName);
    return Msg::Result{true};
}

void Server::addUser(const std::string& p_userName)
{
    allUsers.insert({p_userName, std::make_shared<User>(connections, p_userName)});
}

void Server::handle(const Msg::UnRegister& p_msg)
{
    auto it = loggedUsers.find(Cookie{p_msg.cookie});
    if(it != loggedUsers.end())
    {
        it->second->offline();
        authenticator->removeUser(it->second->getName());
        allUsers.erase(it->second->getName());
        loggedUsers.erase(it);
        cookies.releaseCookie(Cookie{p_msg.cookie});
    }
}

Msg::Cookie Server::handle(const Msg::Login& p_msg)
try
{
    if(!authenticator->authenticate(p_msg.userName, p_msg.password))
    {
        return Msg::Cookie{""};
    }
    auto user = allUsers.find(p_msg.userName);
    if(user == allUsers.end())
    {
        return Msg::Cookie{""};
    }
    if(user->second->isLogged())
    {
        logOut(user->second->getCookie());
    }
    auto cookie = cookies.allocateCookie();
    user->second->login(cookie);
    loggedUsers[cookie] = user->second;
    return Msg::Cookie{ cookie.toString() };
}
catch(std::exception& e)
{
    std::cerr << "Chatter: Failed to authenticate user: " << p_msg.userName << ". " << e.what() << std::endl;
    return Msg::Cookie{""};
}

void Server::logOut(const Cookie& p_cookie)
{
    auto user = loggedUsers.find(p_cookie);
    if(user != loggedUsers.end())
    {
        user->second->offline();
        user->second->logout();
        loggedUsers.erase(user);
        cookies.releaseCookie(p_cookie);
    }
}

void Server::handle(const Msg::Logout& p_msg)
{
    logOut(Cookie{p_msg.cookie});
}

Msg::Result Server::handle(const Msg::OnLine& p_msg)
{
    auto user = loggedUsers.find(Cookie{p_msg.cookie});
    if(user == loggedUsers.end())
    {
        return Msg::Result{false};
    }
    user->second->online(p_msg.host, p_msg.port);
    return Msg::Result{true};
}

void Server::handle(const Msg::OffLine& p_msg)
{
    auto user = loggedUsers.find(Cookie{p_msg.cookie});
    if(user == loggedUsers.end())
    {
        return;
    }
    user->second->offline();
}

Msg::MessageAck Server::handle(const Msg::Message& p_msg)
{
    decltype(loggedUsers)::iterator sender;
    decltype(allUsers)::iterator receiver;
    if ((sender = loggedUsers.find(Cookie{p_msg.from})) == loggedUsers.end()
     || (receiver = allUsers.find(p_msg.to)) == allUsers.end())
    {
        return Msg::MessageAck{Msg::MessageAck::Status::UnknownUser};
    }
    auto message = p_msg;
    message.from = sender->second->getName();
    return Msg::MessageAck{ receiver->second->message(message) };
}

} // namespace Chatter
