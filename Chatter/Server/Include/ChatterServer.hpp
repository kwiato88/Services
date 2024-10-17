#pragma once

#include <string>
#include <map>
#include <queue>
#include "ChatterMsg.hpp"
#include "ChatterCookie.hpp"
#include "MsgReqHandler.hpp"

namespace Chatter
{

class Server
 : public msg::ReqHandler<Msg::Register, Msg::Cookie>,
   public msg::IndHandler<Msg::UnRegister>,
   public msg::ReqHandler<Msg::OnLine, Msg::Result>,
   public msg::IndHandler<Msg::OffLine>,
   public msg::ReqHandler<Msg::Message, Msg::MessageAck>
{
public:
    Msg::Cookie handle(const Msg::Register& p_msg);
    void handle(const Msg::UnRegister& p_msg);
    Msg::Result handle(const Msg::OnLine& p_msg);
    void handle(const Msg::OffLine& p_msg);
    Msg::MessageAck handle(const Msg::Message& p_msg);

private:
    class User
    {
    public:
        User(const std::string& p_name = "");
        bool is(const std::string& p_name) const;
        void online(const std::string& p_host, const std::string& p_port);
        void offline();
        Msg::MessageAck::Status message(const Msg::Message& p_message);
        
    private:
        Msg::MessageAck::Status sendMsg(const Msg::Message& p_message);
        Msg::MessageAck::Status bufferMsg(const Msg::Message& p_message);
        void sendBufferedMessages();
        
        using SendFunction = Msg::MessageAck::Status (User::*)(const Msg::Message&);
        std::string name;
        std::string host;
        std::string port;
        SendFunction sendAction;
        std::queue<Msg::Message> bufferedMessages;
        static const std::size_t bufferCapacity = 512;
    };

    bool isRegistered(const Cookie& p_cookie) const;
    bool isRegistered(const std::string& p_userName) const;
    User& getUser(const std::string& p_userName);

    CookieStore cookies;
    std::map<Cookie, User> users;
};

}
