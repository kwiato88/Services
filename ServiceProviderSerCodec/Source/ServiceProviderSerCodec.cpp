#include "ServiceProviderSerCodec.hpp"

#include <iostream>
#include <sstream>

namespace Networking
{
namespace ServiceProviderMsg
{
namespace Json
{

ID IdConverter::fromString(const std::string& p_str)
{
	return Networking::ServiceProviderMsg::idFromString(p_str);
}
std::string IdConverter::toString(ID p_id)
{
	return Networking::ServiceProviderMsg::toString(p_id);
}

boost::property_tree::ptree encode_GetServiceAddr(const GetServiceAddr& p_msg)
{
	boost::property_tree::ptree msg;
	msg.put("name", p_msg.name);
	return msg;
}
GetServiceAddr decode_GetServiceAddr(const boost::property_tree::ptree& p_msg)
{
	GetServiceAddr msg = {};
	msg.name = p_msg.get<std::string>("name", "");
	return msg;
}

boost::property_tree::ptree encode_ServiceAddr(const ServiceAddr& p_msg)
{
	boost::property_tree::ptree msg;
	msg.put("host", p_msg.host);
	msg.put("port", p_msg.port);
	return msg;
}
ServiceAddr decode_ServiceAddr(const boost::property_tree::ptree& p_msg)
{
	ServiceAddr msg = {};
	msg.host = p_msg.get<std::string>("host", "");
	msg.port = p_msg.get<std::string>("port", "");
	return msg;
}

boost::property_tree::ptree encode_SetService(const SetService& p_msg)
{
	boost::property_tree::ptree msg;
	msg.put("name", p_msg.name);
	msg.put("host", p_msg.host);
	msg.put("port", p_msg.port);
	return msg;
}
SetService decode_SetService(const boost::property_tree::ptree& p_msg)
{
	SetService msg = {};
	msg.name = p_msg.get<std::string>("name", "");
	msg.host = p_msg.get<std::string>("host", "");
	msg.port = p_msg.get<std::string>("port", "");
	return msg;
}

boost::property_tree::ptree encode_RemoveService(const RemoveService& p_msg)
{
	boost::property_tree::ptree msg;
	msg.put("name", p_msg.name);
	return msg;
}
RemoveService decode_RemoveService(const boost::property_tree::ptree& p_msg)
{
	RemoveService msg = {};
	msg.name = p_msg.get<std::string>("name", "");
	return msg;
}

boost::property_tree::ptree encode_Stop(const Stop&)
{
	return boost::property_tree::ptree();
}
Stop decode_Stop(const boost::property_tree::ptree&)
{
	return Stop{};
}

template<>
boost::property_tree::ptree Serialization::toPtree<GetServiceAddr>(const GetServiceAddr& p_msg)
{
	return encode_GetServiceAddr(p_msg);
}
template<>
GetServiceAddr Serialization::fromPtree<GetServiceAddr>(const boost::property_tree::ptree& p_data)
{
	return decode_GetServiceAddr(p_data);
}

template<>
boost::property_tree::ptree Serialization::toPtree<ServiceAddr>(const ServiceAddr& p_msg)
{
	return encode_ServiceAddr(p_msg);
}
template<>
ServiceAddr Serialization::fromPtree<ServiceAddr>(const boost::property_tree::ptree& p_data)
{
	return decode_ServiceAddr(p_data);
}

template<>
boost::property_tree::ptree Serialization::toPtree<SetService>(const SetService& p_msg)
{
	return encode_SetService(p_msg);
}
template<>
SetService Serialization::fromPtree<SetService>(const boost::property_tree::ptree& p_data)
{
	return decode_SetService(p_data);
}

template<>
boost::property_tree::ptree Serialization::toPtree<RemoveService>(const RemoveService& p_msg)
{
	return encode_RemoveService(p_msg);
}
template<>
RemoveService Serialization::fromPtree<RemoveService>(const boost::property_tree::ptree& p_data)
{
	return decode_RemoveService(p_data);
}

template<>
boost::property_tree::ptree Serialization::toPtree<Stop>(const Stop& p_msg)
{
	return encode_Stop(p_msg);
}
template<>
Stop Serialization::fromPtree<Stop>(const boost::property_tree::ptree& p_data)
{
	return decode_Stop(p_data);
}

}
}
}
