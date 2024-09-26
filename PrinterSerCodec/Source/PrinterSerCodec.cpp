#include "PrinterSerCodec.hpp"
#include "JsonCodec.hpp"

namespace Networking
{
namespace PrinterSerMsg
{
namespace Json
{

ID IdConverter::fromString(const std::string& p_str)
{
	return Networking::PrinterSerMsg::idFromString(p_str);
}
std::string IdConverter::toString(ID p_id)
{
	return Networking::PrinterSerMsg::toString(p_id);
}

Print decode_Print(const boost::property_tree::ptree& p_data)
{
	Print msg = {};
	msg.data = p_data.get<std::string>("data");
	return msg;
}
boost::property_tree::ptree encode_Print(const Print& p_msg)
{
	boost::property_tree::ptree msg;
	msg.put("data", p_msg.data);
	return msg;
}

Stop decode_Stop(const boost::property_tree::ptree&)
{
	return Stop{};
}
boost::property_tree::ptree encode_Stop(const Stop&)
{
	return boost::property_tree::ptree();
}
template<>
boost::property_tree::ptree Serialization::toPtree<Print>(const Print& p_msg)
{
	return encode_Print(p_msg);
}
template<>
Print Serialization::fromPtree<Print>(const boost::property_tree::ptree& p_data)
{
	return decode_Print(p_data);
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
