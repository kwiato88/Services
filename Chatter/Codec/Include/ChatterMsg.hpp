#pragma once

#include <string>
#include <ostream>

namespace Chatter
{
namespace Msg
{

enum class ID
{
    Register,
    Cookie,
    Result,
    UnRegister,
    OnLine,
    OffLine,
    Message,
    MessageAck,
    Stop,
    Dummy
};
std::string toString(ID p_id);
ID idFromString(const std::string& p_id);

struct Register
{
    static const ID id = ID::Register;
    std::string userName;
};
struct Cookie
{
    static const ID id = ID::Cookie;
    std::string cookie;
};
struct Result
{
    static const ID id = ID::Result;
    bool success;
};
struct UnRegister
{
    static const ID id = ID::UnRegister;
    std::string cookie;
};
struct OnLine
{
    static const ID id = ID::OnLine;
    std::string cookie;
    std::string host;
    std::string port;
};
struct OffLine
{
    static const ID id = ID::OffLine;
    std::string cookie;
};
struct Message
{
    static const ID id = ID::Message;
    std::string from;
    std::string to;
    std::string message;
};
struct MessageAck
{
    static const ID id = ID::MessageAck;
    enum class Status
    {
        Sent,
        Buffered,
        UnknownUser,
        Failed
    };
    Status status;
};
struct Stop
{
    static const ID id = ID::Stop;
};

} // namespace Msg
} // namespace Chatter

std::ostream& operator<<(std::ostream& p_out, const Chatter::Msg::ID& p_id);
