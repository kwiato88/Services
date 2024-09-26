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

template<>
boost::property_tree::ptree Serialization::toPtree<GetServiceAddr>(const GetServiceAddr& p_msg)
{
	boost::property_tree::ptree msg;
	msg.put("name", p_msg.name);
	return msg;
}
template<>
GetServiceAddr Serialization::fromPtree<GetServiceAddr>(const boost::property_tree::ptree& p_data)
{
	GetServiceAddr msg = {};
	msg.name = p_data.get<std::string>("name", "");
	return msg;
}

template<>
boost::property_tree::ptree Serialization::toPtree<ServiceAddr>(const ServiceAddr& p_msg)
{
	boost::property_tree::ptree msg;
	msg.put("host", p_msg.host);
	msg.put("port", p_msg.port);
	return msg;
}
template<>
ServiceAddr Serialization::fromPtree<ServiceAddr>(const boost::property_tree::ptree& p_data)
{
	ServiceAddr msg = {};
	msg.host = p_data.get<std::string>("host", "");
	msg.port = p_data.get<std::string>("port", "");
	return msg;
}

template<>
boost::property_tree::ptree Serialization::toPtree<SetService>(const SetService& p_msg)
{
	boost::property_tree::ptree msg;
	msg.put("name", p_msg.name);
	msg.put("host", p_msg.host);
	msg.put("port", p_msg.port);
	return msg;
}
template<>
SetService Serialization::fromPtree<SetService>(const boost::property_tree::ptree& p_data)
{
	SetService msg = {};
	msg.name = p_data.get<std::string>("name", "");
	msg.host = p_data.get<std::string>("host", "");
	msg.port = p_data.get<std::string>("port", "");
	return msg;
}

template<>
boost::property_tree::ptree Serialization::toPtree<RemoveService>(const RemoveService& p_msg)
{
	boost::property_tree::ptree msg;
	msg.put("name", p_msg.name);
	return msg;
}
template<>
RemoveService Serialization::fromPtree<RemoveService>(const boost::property_tree::ptree& p_data)
{
	RemoveService msg = {};
	msg.name = p_data.get<std::string>("name", "");
	return msg;
}

template<>
boost::property_tree::ptree Serialization::toPtree<Stop>(const Stop&)
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
