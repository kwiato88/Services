#include "ChatterMsg.hpp"

namespace Chatter
{
namespace Msg
{

std::string toString(ID p_id)
{
    switch (p_id)
    {
    case ID::Register: return "Register";
    case ID::Cookie: return "Cookie";
    case ID::Result: return "Result";
    case ID::UnRegister: return "UnRegister";
    case ID::OnLine: return "OnLine";
    case ID::OffLine: return "OffLine";
    case ID::Message: return "Message";
    case ID::MessageAck: return "MessageAck";
    case ID::Stop: return "Stop";
    case ID::Dummy: return "Dummy";
    default: throw std::runtime_error("Invalid ID");
    }

}

ID idFromString(const std::string& p_id)
{
    if (p_id == "Register")
        return ID::Register;
    if (p_id == "Cookie")
        return ID::Cookie;
    if (p_id == "Result")
        return ID::Result;
    if (p_id == "UnRegister")
        return ID::UnRegister;
    if (p_id == "OnLine")
        return ID::OnLine;
    if (p_id == "OffLine")
        return ID::OffLine;
    if (p_id == "Message")
        return ID::Message;
    if (p_id == "MessageAck")
        return ID::MessageAck;
    if (p_id == "Stop")
        return ID::Stop;
    if (p_id == "Dummy")
        return ID::Dummy;
    throw std::runtime_error(std::string("Invalid ID: ") + p_id);
}

}
}

std::ostream& operator<<(std::ostream& p_out, const Chatter::Msg::ID& p_id)
{
    p_out << Chatter::Msg::toString(p_id);
    return p_out;
}
