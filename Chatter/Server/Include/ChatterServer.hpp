#pragma once

#include <string>
#include <map>
#include <queue>
#include <memory>
#include <functional>
#include "ChatterMsg.hpp"
#include "ChatterCookie.hpp"
#include "ChatterIAuthenticator.hpp"
#include "MsgReqHandler.hpp"
#include "MsgConnection.hpp"

namespace Chatter
{

using ConnectionFactory = std::function<std::unique_ptr<msg::Connection>(const std::string& host, const std::string& port)>;

class Server
 : public msg::ReqHandler<Msg::Register, Msg::Result>,
   public msg::IndHandler<Msg::UnRegister>,
   public msg::ReqHandler<Msg::Login, Msg::Cookie>,
   public msg::IndHandler<Msg::Logout>,
   public msg::ReqHandler<Msg::OnLine, Msg::Result>,
   public msg::IndHandler<Msg::OffLine>,
   public msg::ReqHandler<Msg::Message, Msg::MessageAck>
{
public:
    Server(ConnectionFactory p_factory, std::unique_ptr<IAuthenticator> p_authenticator);
    Msg::Result handle(const Msg::Register& p_msg);
    void handle(const Msg::UnRegister& p_msg);
    Msg::Cookie handle(const Msg::Login& p_msg);
    void handle(const Msg::Logout& p_msg);
    Msg::Result handle(const Msg::OnLine& p_msg);
    void handle(const Msg::OffLine& p_msg);
    Msg::MessageAck handle(const Msg::Message& p_msg);

private:
    class User
    {
    public:
        User() = default;
        User(ConnectionFactory p_factory, const std::string& p_name);
        User(const User&) = default;
        User(User&&) = default;
        User& operator=(const User&) = default;
        User& operator=(User&&) = default;
        bool is(const std::string& p_name) const;
        std::string getName() const;
        void online(const std::string& p_host, const std::string& p_port);
        void offline();
        void login();
        void logout();
        bool isLogged() const;
        Msg::MessageAck::Status message(const Msg::Message& p_message);
        
    private:
        Msg::MessageAck::Status sendMsg(const Msg::Message& p_message);
        Msg::MessageAck::Status bufferMsg(const Msg::Message& p_message);
        void sendBufferedMessages();
        
        using SendFunction = Msg::MessageAck::Status (User::*)(const Msg::Message&);
        ConnectionFactory connections;
        std::string name;
        std::string host;
        std::string port;
        bool logged;
        SendFunction sendAction;
        std::queue<Msg::Message> bufferedMessages;
        static const std::size_t bufferCapacity = 512;
    };

    void addUser(const std::string& p_userName);
    bool isLogged(const Cookie& p_cookie) const;
    bool isLogged(const std::string& p_userName) const;
    bool isRegistered(const std::string& p_userName) const;

    ConnectionFactory connections;
    CookieStore cookies;
    std::unique_ptr<IAuthenticator> authenticator;
    std::map<std::string, std::shared_ptr<User> > allUsers;
    std::map<Cookie, std::shared_ptr<User> > loggedUsers;
};

}
