#pragma once

#include "ServiceProviderSerMsg.hpp"
#include "JsonCodec.hpp"

#include <stdexcept>
#include <boost/property_tree/ptree.hpp>

namespace Networking
{
namespace ServiceProviderMsg
{
namespace Json
{

struct IdConverter
{
	typedef Networking::ServiceProviderMsg::ID IdType;
	static const ID deafultId = ID::Dummy;
	static ID fromString(const std::string& p_str);
	static std::string toString(ID p_id);
};

boost::property_tree::ptree encode_GetServiceAddr(const GetServiceAddr&);
boost::property_tree::ptree encode_ServiceAddr(const ServiceAddr&);
boost::property_tree::ptree encode_SetService(const SetService&);
boost::property_tree::ptree encode_RemoveService(const RemoveService&);
boost::property_tree::ptree encode_Stop(const Stop&);
GetServiceAddr decode_GetServiceAddr(const boost::property_tree::ptree&);
ServiceAddr decode_ServiceAddr(const boost::property_tree::ptree&);
SetService  decode_SetService(const boost::property_tree::ptree&);
RemoveService decode_RemoveService(const boost::property_tree::ptree&);
Stop decode_Stop(const boost::property_tree::ptree&);

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

	template<>
	static boost::property_tree::ptree toPtree<GetServiceAddr>(const GetServiceAddr& p_msg)
	{
		return encode_GetServiceAddr(p_msg);
	}
	template<>
	static GetServiceAddr fromPtree<GetServiceAddr>(const boost::property_tree::ptree& p_data)
	{
		return decode_GetServiceAddr(p_data);
	}

	template<>
	static boost::property_tree::ptree toPtree<ServiceAddr>(const ServiceAddr& p_msg)
	{
		return encode_ServiceAddr(p_msg);
	}
	template<>
	static ServiceAddr fromPtree<ServiceAddr>(const boost::property_tree::ptree& p_data)
	{
		return decode_ServiceAddr(p_data);
	}

	template<>
	static boost::property_tree::ptree toPtree<SetService>(const SetService& p_msg)
	{
		return encode_SetService(p_msg);
	}
	template<>
	static SetService fromPtree<SetService>(const boost::property_tree::ptree& p_data)
	{
		return decode_SetService(p_data);
	}

	template<>
	static boost::property_tree::ptree toPtree<RemoveService>(const RemoveService& p_msg)
	{
		return encode_RemoveService(p_msg);
	}
	template<>
	static RemoveService fromPtree<RemoveService>(const boost::property_tree::ptree& p_data)
	{
		return decode_RemoveService(p_data);
	}

	template<>
	static boost::property_tree::ptree toPtree<Stop>(const Stop& p_msg)
	{
		return encode_Stop(p_msg);
	}
	template<>
	static Stop fromPtree<Stop>(const boost::property_tree::ptree& p_data)
	{
		return decode_Stop(p_data);
	}
};

using Codec = ::JsonCodec::Codec<IdConverter, Serialization>;

}
}
}
