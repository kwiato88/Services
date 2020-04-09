#pragma once

#include <string>
#include "ServiceProviderClient.hpp"

namespace Networking
{

class ClientStore
{
public:
	msg::Client get(const std::string& p_serviceName);

private:
	Networking::ServiceProviderClient addrs;
};

}
