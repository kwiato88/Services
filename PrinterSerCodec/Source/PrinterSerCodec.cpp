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

template<>
boost::property_tree::ptree Serialization::toPtree<Print>(const Print& p_msg)
{
	boost::property_tree::ptree msg;
	msg.put("data", p_msg.data);
	return msg;
}
template<>
Print Serialization::fromPtree<Print>(const boost::property_tree::ptree& p_data)
{
	Print msg = {};
	msg.data = p_data.get<std::string>("data");
	return msg;
}
template<>
boost::property_tree::ptree Serialization::toPtree<Stop>(const Stop& p_msg)
{
	return boost::property_tree::ptree();
}
template<>
Stop Serialization::fromPtree<Stop>(const boost::property_tree::ptree& p_data)
{
	return Stop{};
}

}
}
}
