#pragma once

#include "ServiceProviderClient.hpp"
#include "ClientWithCodec.hpp"
#include "MsgClient.hpp"

#include <map>

namespace Networking
{

class CachedClientStore
{
public:
	CachedClientStore(const std::string& p_providerHost = "127.0.0.1", const std::string& p_providerPort = "50000");

	msg::Client get(const std::string& p_serviceName);
	template<typename Codec>
	ClientWithCodec<Codec> getWtihCodec(const std::string& p_serviceName)
	{
		return ClientWithCodec<Codec>(get(p_serviceName));
	}
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
