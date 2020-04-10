#pragma once

#include "ServiceProviderClient.hpp"
#include "MsgClient.hpp"

#include <map>

namespace Networking
{

class CachedClientStore
{
public:
	msg::Client get(const std::string& p_serviceName);
	void remove(const std::string& p_serviceName);
	void refresh(const std::string& p_serviceName);

private:
	struct Addr
	{
		Addr(const ServiceProviderMsg::ServiceAddr&);

		std::string host;
		std::string port;
	};

	bool testConnection(const Addr&) const;
	msg::Client buildClient(const Addr&) const;

	ServiceProviderClient provider;
	std::map<std::string, Addr> addrs;
};

}
