#include "ServiceProviderClient.hpp"
#include "MsgTcpIpConnection.hpp"

namespace Networking
{

ServiceProviderClient::ServiceProviderClient()
	: ServiceProviderClient("127.0.0.1", "50000")
{}

ServiceProviderClient::ServiceProviderClient(const std::string& p_serverHost, const std::string& p_serverPort)
	: client(msg::Client([=]() { return std::make_unique<msg::TcpIpConnection>(p_serverHost, p_serverPort); }))
{}

ServiceProviderMsg::ServiceAddr ServiceProviderClient::getServiceAddr(const std::string& p_name)
{
	using namespace ServiceProviderMsg;
	GetServiceAddr req = {};
	req.name = p_name;
	return client.sendReq<GetServiceAddr, ServiceAddr>(req);
}

ServiceProviderMsg::ServiceAddr ServiceProviderClient::setServiceAddr(const std::string& p_name)
{
	using namespace ServiceProviderMsg;
	SetService req = {};
	req.name = p_name;
	return client.sendReq<SetService, ServiceAddr>(req);
}

ServiceProviderMsg::ServiceAddr ServiceProviderClient::setServiceAddr(const std::string& p_service, const std::string& p_host, const std::string& p_port)
{
	using namespace ServiceProviderMsg;
	SetService req = {};
	req.name = p_service;
	req.host = p_host;
	req.port = p_port;
	return client.sendReq<SetService, ServiceAddr>(req);
}

void ServiceProviderClient::removeServiceAddr(const std::string& p_name)
{
	using namespace ServiceProviderMsg;
	RemoveService msg = {};
	msg.name = p_name;
	client.sendInd(msg);
}

void ServiceProviderClient::stop()
{
	using namespace ServiceProviderMsg;
	client.sendInd(Stop{});
}

}
