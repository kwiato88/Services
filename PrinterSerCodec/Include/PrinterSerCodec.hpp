#pragma once

#include "PrinterSerMsg.hpp"
#include "JsonCodec.hpp"

#include <boost/property_tree/ptree.hpp>

namespace Networking
{
namespace PrinterSerMsg
{
namespace Json
{

struct IdConverter
{
	typedef Networking::PrinterSerMsg::ID IdType;
	static const ID deafultId = ID::Dummy;
	static ID fromString(const std::string& p_str);
	static std::string toString(ID p_id);
};

Print decode_Print(const boost::property_tree::ptree&);
boost::property_tree::ptree encode_Print(const Print&);

Stop decode_Stop(const boost::property_tree::ptree&);
boost::property_tree::ptree encode_Stop(const Stop&);

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
	static boost::property_tree::ptree toPtree<Print>(const Print& p_msg)
	{
		return encode_Print(p_msg);
	}
	template<>
	static Print fromPtree<Print>(const boost::property_tree::ptree& p_data)
	{
		return decode_Print(p_data);
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
