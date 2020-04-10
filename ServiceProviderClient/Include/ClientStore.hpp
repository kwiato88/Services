#pragma once

#include <string>
#include "ServiceProviderClient.hpp"
#include "ClientWithCodec.hpp"

namespace Networking
{

class ClientStore
{
public:
	ClientStore(const std::string& p_providerHost = "127.0.0.1", const std::string& p_providerPort = "50000");
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
