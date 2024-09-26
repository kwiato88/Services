#include <stdexcept>
#include "PrinterSerMsg.hpp"

namespace Networking
{
namespace PrinterSerMsg
{

std::string toString(ID p_id)
{
	switch (p_id)
	{
	case ID::Print : return "Print";
	case ID::Stop: return "Stop";
	case ID::Dummy: return "Dummy";
	default: throw std::runtime_error("Invalid ID");
	}
}
ID idFromString(const std::string& p_id)
{
	if (p_id == "Print")
		return ID::Print;
	if (p_id == "Stop")
		return ID::Stop;
	if (p_id == "Dummy")
		return ID::Dummy;
	throw std::runtime_error(std::string("Invalid ID: ") + p_id);
}

}
}
