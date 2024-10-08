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
boost::property_tree::ptree Serialization::toPtree<GetServiceAddr>(const GetServiceAddr& p_msg);
template<>
GetServiceAddr Serialization::fromPtree<GetServiceAddr>(const boost::property_tree::ptree& p_data);
template<>
boost::property_tree::ptree Serialization::toPtree<ServiceAddr>(const ServiceAddr& p_msg);
template<>
ServiceAddr Serialization::fromPtree<ServiceAddr>(const boost::property_tree::ptree& p_data);
template<>
boost::property_tree::ptree Serialization::toPtree<SetService>(const SetService& p_msg);
template<>
SetService Serialization::fromPtree<SetService>(const boost::property_tree::ptree& p_data);
template<>
boost::property_tree::ptree Serialization::toPtree<RemoveService>(const RemoveService& p_msg);
template<>
RemoveService Serialization::fromPtree<RemoveService>(const boost::property_tree::ptree& p_data);
template<>
boost::property_tree::ptree Serialization::toPtree<Stop>(const Stop& p_msg);
template<>
Stop Serialization::fromPtree<Stop>(const boost::property_tree::ptree& p_data);

using Codec = ::JsonCodec::Codec<IdConverter, Serialization>;

}
}
}
