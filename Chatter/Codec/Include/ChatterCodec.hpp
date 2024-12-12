#pragma once

#include "ChatterMsg.hpp"
#include "JsonCodec.hpp"

namespace Chatter
{
namespace Msg
{
namespace Json
{

struct IdConverter
{
    typedef Chatter::Msg::ID IdType;
    static const ID deafultId = ID::Dummy;
    static ID fromString(const std::string& p_str);
    static std::string toString(ID p_id);
};

struct Serialization
{
	template<typename T>
	static boost::property_tree::ptree toPtree(const T&)
	{
		throw ::JsonCodec::CodecError("Unextected message to encode");
	}
	template<typename T>
	static T fromPtree(const boost::property_tree::ptree&)
	{
		throw ::JsonCodec::CodecError("Unextected message to decode");
	}
};

template<>
boost::property_tree::ptree Serialization::toPtree<Register>(const Register& p_msg);
template<>
Register Serialization::fromPtree<Register>(const boost::property_tree::ptree& p_data);
template<>
boost::property_tree::ptree Serialization::toPtree<Cookie>(const Cookie& p_msg);
template<>
Cookie Serialization::fromPtree<Cookie>(const boost::property_tree::ptree& p_data);
template<>
boost::property_tree::ptree Serialization::toPtree<Result>(const Result& p_msg);
template<>
Result Serialization::fromPtree<Result>(const boost::property_tree::ptree& p_data);
template<>
boost::property_tree::ptree Serialization::toPtree<UnRegister>(const UnRegister& p_msg);
template<>
UnRegister Serialization::fromPtree<UnRegister>(const boost::property_tree::ptree& p_data);
template<>
boost::property_tree::ptree Serialization::toPtree<Login>(const Login& p_msg);
template<>
Login Serialization::fromPtree<Login>(const boost::property_tree::ptree& p_data);
template<>
boost::property_tree::ptree Serialization::toPtree<Logout>(const Logout& p_msg);
template<>
Logout Serialization::fromPtree<Logout>(const boost::property_tree::ptree& p_data);
template<>
boost::property_tree::ptree Serialization::toPtree<OnLine>(const OnLine& p_msg);
template<>
OnLine Serialization::fromPtree<OnLine>(const boost::property_tree::ptree& p_data);
template<>
boost::property_tree::ptree Serialization::toPtree<OffLine>(const OffLine& p_msg);
template<>
OffLine Serialization::fromPtree<OffLine>(const boost::property_tree::ptree& p_data);
template<>
boost::property_tree::ptree Serialization::toPtree<Message>(const Message& p_msg);
template<>
Message Serialization::fromPtree<Message>(const boost::property_tree::ptree& p_data);
template<>
boost::property_tree::ptree Serialization::toPtree<MessageAck>(const MessageAck& p_msg);
template<>
MessageAck Serialization::fromPtree<MessageAck>(const boost::property_tree::ptree& p_data);
template<>
boost::property_tree::ptree Serialization::toPtree<Stop>(const Stop& p_msg);
template<>
Stop Serialization::fromPtree<Stop>(const boost::property_tree::ptree& p_data);

using Codec = ::JsonCodec::Codec<IdConverter, Serialization>;

}
}
}
