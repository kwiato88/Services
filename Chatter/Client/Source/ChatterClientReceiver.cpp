#include <iostream>
#include "ChatterClientReceiver.hpp"
#include "MsgTcpIpServer.hpp"
#include "MsgReqHandler.hpp"

namespace Chatter
{

class MessagePrinter : public msg::IndHandler<Msg::Message>
{
public:
    void handle(const Msg::Message& p_msg) override
    {
        std::cout << "=== " << p_msg.from << " ===\n" << p_msg.message << std::endl;
    }
};

ClientReceiver::ClientReceiver(const std::string& p_host, const std::string& p_port)
    : BaseService([=]() { return std::make_unique<msg::TcpIpServer>(p_host, p_port); })
{
    addHandler<Msg::Message>(Msg::Message::id, std::make_shared<MessagePrinter>());
    addHandler<Msg::Stop>(Msg::Stop::id, std::make_shared<StopHandler<Msg::Stop> >(*this));
}

}
