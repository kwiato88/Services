#include <iostream>
#include "ChatterClientReceiver.hpp"
#include "MsgTcpIpServer.hpp"
#include "MsgReqHandler.hpp"

namespace Chatter
{

class MessageHandler : public msg::IndHandler<Msg::Message>
{
public:
    MessageHandler(std::function<void(const Msg::Message&)> p_onMessageReceived)
    : onMessageReceived(p_onMessageReceived)
    {}
    void handle(const Msg::Message& p_msg) override
    {
        onMessageReceived(p_msg);
    }
private:
    std::function<void(const Msg::Message&)> onMessageReceived;
};

ClientReceiver::ClientReceiver(const std::string& p_host, const std::string& p_port,
    std::function<void(const Msg::Message&)> p_onMessageReceived)
    : BaseService([=]() { return std::make_unique<msg::TcpIpServer>(p_host, p_port); })
{
    addHandler<Msg::Message>(Msg::Message::id, std::make_shared<MessageHandler>(p_onMessageReceived));
    addHandler<Msg::Stop>(Msg::Stop::id, std::make_shared<StopHandler<Msg::Stop> >(*this));
}

}
