#include "ServiceProviderSerMsg.hpp"

#include <iostream>
#include <stdexcept>

namespace Networking
{
namespace ServiceProviderMsg
{

std::string toString(ID p_id)
{
	switch (p_id)
	{
	case ID::GetServiceAddr: return "GetServiceAddr";
	case ID::ServiceAddr: return "ServiceAddr";
	case ID::SetService: return "SetService";
	case ID::Stop: return "Stop";
	case ID::Dummy: return "Dummy";
	default: throw std::runtime_error("Invalid ID");
	}
}

ID idFromString(const std::string& p_id)
{
	//std::cout << "ID FROM STR: " << p_id << std::endl;
	if (p_id == "GetServiceAddr")
		return ID::GetServiceAddr;
	if (p_id == "ServiceAddr")
		return ID::ServiceAddr;
	if (p_id == "SetService")
		return ID::SetService;
	if (p_id == "Stop")
		return ID::Stop;
	if (p_id == "Dummy")
		return ID::Dummy;
	throw std::runtime_error(std::string("Invalid ID: ") + p_id);
}

}
}

std::ostream& operator<<(std::ostream& p_out, const Networking::ServiceProviderMsg::ID& p_id)
{
	p_out << Networking::ServiceProviderMsg::toString(p_id);
	return p_out;
}
