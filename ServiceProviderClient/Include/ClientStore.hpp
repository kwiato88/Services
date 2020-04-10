#pragma once

#include <string>
#include "ServiceProviderClient.hpp"
#include "ClientWithCodec.hpp"

namespace Networking
{

class ClientStore
{
public:
	msg::Client get(const std::string& p_serviceName);
	
	template<typename Codec>
	ClientWithCodec<Codec> getWithCodec(const std::string& p_serviceName)
	{
		return ClientWithCodec<Codec>(get(p_serviceName));
	}
private:
	Networking::ServiceProviderClient addrs;
};

}
