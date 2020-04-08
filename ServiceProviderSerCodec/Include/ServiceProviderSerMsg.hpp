#pragma once

#include <string>
#include <ostream>

namespace Networking
{
namespace ServiceProviderMsg
{

enum class ID
{
	GetServiceAddr,
	ServiceAddr,
	SetService,
	Stop,
	Dummy
};

std::string toString(ID p_id);
ID idFromString(const std::string& p_id);

struct GetServiceAddr
{
	static const ID id = ID::GetServiceAddr;
	std::string name;
};

struct ServiceAddr
{
	static const ID id = ID::ServiceAddr;
	std::string host;
	std::string port;
};

struct SetService
{
	static const ID id = ID::SetService;
	std::string name;
	std::string host;
	std::string port;
};

struct Stop
{
	static const ID id = ID::Stop;
};

}
}

std::ostream& operator<<(std::ostream& p_out, const Networking::ServiceProviderMsg::ID& p_id);
