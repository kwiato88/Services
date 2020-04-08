#include "ServiceProviderSerCodec.hpp"

#include <iostream>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace Networking
{
namespace ServiceProviderMsg
{
namespace Json
{

ID getId(const std::string& p_data)
{
	//std::cout << "GET ID: " << p_data << std::endl;
	std::stringstream buff;
	buff << p_data;
	boost::property_tree::ptree data;
	boost::property_tree::json_parser::read_json(buff, data);
	return idFromString(data.get<std::string>("ID", ""));
}

std::string encode_Message(ID p_id, const boost::property_tree::ptree& p_data)
{
	boost::property_tree::ptree data;
	data.put("ID", toString(p_id));
	data.add_child("msg", p_data);

	std::stringstream buffer;
	boost::property_tree::json_parser::write_json(buffer, data);
	//std::cout << "ENCODED: " << buffer.str() << std::endl;
	return buffer.str();
}
boost::property_tree::ptree decode_Message(ID p_expectedId, const std::string& p_data)
{
	//std::cout << "TO DECODE: " << p_data << std::endl;
	std::stringstream buff;
	buff << p_data;
	boost::property_tree::ptree data;
	boost::property_tree::json_parser::read_json(buff, data);

	if (p_expectedId != idFromString(data.get<std::string>("ID", "")))
		throw CodecError(std::string("Unexpected ID. Expected: ") + toString(p_expectedId) + ". Got: " + data.get<std::string>("ID", ""));
	return data.get_child("msg");
}

//TODO: consider adding messages content validation

std::string encode_GetServiceAddr(const GetServiceAddr& p_msg)
{
	boost::property_tree::ptree msg;
	msg.put("name", p_msg.name);

	return encode_Message(ID::GetServiceAddr, msg);
}
GetServiceAddr decode_GetServiceAddr(const std::string& p_msg)
{
	auto data = decode_Message(ID::GetServiceAddr, p_msg);
	GetServiceAddr msg = {};
	msg.name = data.get<std::string>("name", "");
	return msg;
}

std::string encode_ServiceAddr(const ServiceAddr& p_msg)
{
	boost::property_tree::ptree msg;
	msg.put("host", p_msg.host);
	msg.put("port", p_msg.port);

	return encode_Message(ID::ServiceAddr, msg);
}
ServiceAddr decode_ServiceAddr(const std::string& p_msg)
{
	auto data = decode_Message(ID::ServiceAddr, p_msg);
	ServiceAddr msg = {};
	msg.host = data.get<std::string>("host", "");
	msg.port = data.get<std::string>("port", "");
	return msg;
}

std::string encode_SetService(const SetService& p_msg)
{
	boost::property_tree::ptree msg;
	msg.put("name", p_msg.name);
	msg.put("host", p_msg.host);
	msg.put("port", p_msg.port);

	return encode_Message(ID::SetService, msg);
}
SetService decode_SetService(const std::string& p_msg)
{
	auto data = decode_Message(ID::SetService, p_msg);
	SetService msg = {};
	msg.name = data.get<std::string>("name", "");
	msg.host = data.get<std::string>("host", "");
	msg.port = data.get<std::string>("port", "");
	return msg;
}

std::string encode_Stop(const Stop&)
{
	return encode_Message(ID::Stop, boost::property_tree::ptree());
}
Stop decode_Stop(const std::string& p_msg)
{
	decode_Message(ID::Stop, p_msg);
	return Stop{};
}

}
}
}
