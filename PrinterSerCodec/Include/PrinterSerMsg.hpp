#pragma once

#include <string>

namespace Networking
{
namespace PrinterSerMsg
{
enum class ID
{
	Print,
	Stop,
	Dummy
};

std::string toString(ID p_id);
ID idFromString(const std::string& p_id);

struct Print
{
	static const ID id = ID::Print;
	std::string data;
};

struct Stop
{
	static const ID id = ID::Stop;
};

}
}
