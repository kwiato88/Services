#pragma once

#include <string>
#include "ServiceProviderSerMsg.hpp"
#include "ServiceProviderSerCodec.hpp"
#include "ClientWithCodec.hpp"
#include "MsgClient.hpp"

namespace Networking
{

class ServiceProviderClient
{
public:
	ServiceProviderClient();
	ServiceProviderClient(const std::string& p_serverHost, const std::string& p_serverPort);

	ServiceProviderMsg::ServiceAddr getServiceAddr(const std::string& p_name);
	ServiceProviderMsg::ServiceAddr setServiceAddr(const std::string& p_name);
	ServiceProviderMsg::ServiceAddr setServiceAddr(const std::string& p_service, const std::string& p_host, const std::string& p_port);
	void removeServiceAddr(const std::string& p_name);
	void stop();
private:
	using JsonCodec = ServiceProviderMsg::Json::Codec;
	ClientWithCodec<JsonCodec> client;
};

}
