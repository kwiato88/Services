#include "ChatterCodec.hpp"

namespace Chatter
{
namespace Msg
{
namespace Json
{

ID IdConverter::fromString(const std::string& p_str)
{
    return Chatter::Msg::idFromString(p_str);
}

std::string IdConverter::toString(ID p_id)
{
    return Chatter::Msg::toString(p_id);
}

template<>
boost::property_tree::ptree Serialization::toPtree<Register>(const Register& p_msg)
{
    boost::property_tree::ptree msg;
    msg.put("userName", p_msg.userName);
    return msg;
}
template<>
Register Serialization::fromPtree<Register>(const boost::property_tree::ptree& p_data)
{
    Register msg = {};
    msg.userName = p_data.get<std::string>("userName", "");
    return msg;
}
template<>
boost::property_tree::ptree Serialization::toPtree<Cookie>(const Cookie& p_msg)
{
    boost::property_tree::ptree msg;
    msg.put("cookie", p_msg.cookie);
    return msg;
}
template<>
Cookie Serialization::fromPtree<Cookie>(const boost::property_tree::ptree& p_data)
{
    Cookie msg = {};
    msg.cookie = p_data.get<std::string>("cookie", "");
    return msg;
}
template<>
boost::property_tree::ptree Serialization::toPtree<Result>(const Result& p_msg)
{
    boost::property_tree::ptree msg;
    msg.put("success", p_msg.success);
    return msg;
}
template<>
Result Serialization::fromPtree<Result>(const boost::property_tree::ptree& p_data)
{
    Result msg = {};
    msg.success = p_data.get<bool>("success", false);
    return msg;
}
template<>
boost::property_tree::ptree Serialization::toPtree<UnRegister>(const UnRegister& p_msg)
{
    boost::property_tree::ptree msg;
    msg.put("cookie", p_msg.cookie);
    return msg;
}
template<>
UnRegister Serialization::fromPtree<UnRegister>(const boost::property_tree::ptree& p_data)
{
    UnRegister msg = {};
    msg.cookie = p_data.get<std::string>("cookie", "");
    return msg;
}
template<>
boost::property_tree::ptree Serialization::toPtree<Login>(const Login& p_msg)
{
    boost::property_tree::ptree msg;
    msg.put("userName", p_msg.userName);
    msg.put("password", p_msg.password);
    return msg;
}
template<>
Login Serialization::fromPtree<Login>(const boost::property_tree::ptree& p_data)
{
    Login msg = {};
    msg.userName = p_data.get<std::string>("userName", "");
    msg.password = p_data.get<std::string>("password", "");
    return msg;
}
template<>
boost::property_tree::ptree Serialization::toPtree<Logout>(const Logout& p_msg)
{
    boost::property_tree::ptree msg;
    msg.put("cookie", p_msg.cookie);
    return msg;
}
template<>
Logout Serialization::fromPtree<Logout>(const boost::property_tree::ptree& p_data)
{
    Logout msg = {};
    msg.cookie = p_data.get<std::string>("cookie", "");
    return msg;
}
template<>
boost::property_tree::ptree Serialization::toPtree<OnLine>(const OnLine& p_msg)
{
    boost::property_tree::ptree msg;
    msg.put("cookie", p_msg.cookie);
    msg.put("host", p_msg.host);
    msg.put("port", p_msg.port);
    return msg;
}
template<>
OnLine Serialization::fromPtree<OnLine>(const boost::property_tree::ptree& p_data)
{
    OnLine msg = {};
    msg.cookie = p_data.get<std::string>("cookie", "");
    msg.host = p_data.get<std::string>("host", "");
    msg.port = p_data.get<std::string>("port", "");
    return msg;
}
template<>
boost::property_tree::ptree Serialization::toPtree<OffLine>(const OffLine& p_msg)
{
    boost::property_tree::ptree msg;
    msg.put("cookie", p_msg.cookie);
    return msg;
}
template<>
OffLine Serialization::fromPtree<OffLine>(const boost::property_tree::ptree& p_data)
{
    OffLine msg = {};
    msg.cookie = p_data.get<std::string>("cookie", "");
    return msg;
}
template<>
boost::property_tree::ptree Serialization::toPtree<Message>(const Message& p_msg)
{
    boost::property_tree::ptree msg;
    msg.put("from", p_msg.from);
    msg.put("to", p_msg.to);
    msg.put("message", p_msg.message.substr(0, 1024));
    return msg;
}
template<>
Message Serialization::fromPtree<Message>(const boost::property_tree::ptree& p_data)
{
    Message msg = {};
    msg.from = p_data.get<std::string>("from", "");
    msg.to = p_data.get<std::string>("to", "");
    msg.message = p_data.get<std::string>("message", "");
    return msg;
}
template<>
boost::property_tree::ptree Serialization::toPtree<MessageAck>(const MessageAck& p_msg)
{
    boost::property_tree::ptree msg;
    msg.put("status", static_cast<int>(p_msg.status));
    return msg;
}
template<>
MessageAck Serialization::fromPtree<MessageAck>(const boost::property_tree::ptree& p_data)
{
    MessageAck msg = {};
    msg.status = static_cast<MessageAck::Status>(p_data.get<int>("status", 0));
    return msg;
}
template<>
boost::property_tree::ptree Serialization::toPtree<Stop>(const Stop& p_msg)
{
    return boost::property_tree::ptree();
}
template<>
Stop Serialization::fromPtree<Stop>(const boost::property_tree::ptree&)
{
    return Stop{};
}

}
}
}
