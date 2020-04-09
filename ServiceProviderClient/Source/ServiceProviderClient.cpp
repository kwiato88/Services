#include "ServiceProviderClient.hpp"
#include "MsgTcpIpConnection.hpp"

namespace Networking
{

ServiceProviderClient::ServiceProviderClient()
	: ServiceProviderClient("127.0.0.1", "50000")
{}

ServiceProviderClient::ServiceProviderClient(const std::string& p_serverHost, const std::string& p_serverPort)
	: client([=]() { return std::make_unique<msg::TcpIpConnection>(p_serverHost, p_serverPort); })
{}

ServiceProviderMsg::ServiceAddr ServiceProviderClient::getServiceAddr(const std::string& p_name)
{
	ServiceProviderMsg::GetServiceAddr req = {};
	req.name = p_name;
	return JsonCodec::decode<ServiceProviderMsg::ServiceAddr>(client.sendReq(JsonCodec::encode(req)));
}

ServiceProviderMsg::ServiceAddr ServiceProviderClient::setServiceAddr(const std::string& p_name)
{
	ServiceProviderMsg::SetService req = {};
	req.name = p_name;
	return JsonCodec::decode<ServiceProviderMsg::ServiceAddr>(client.sendReq(JsonCodec::encode(req)));
}

ServiceProviderMsg::ServiceAddr ServiceProviderClient::setServiceAddr(const std::string& p_service, const std::string& p_host, const std::string& p_port)
{
	ServiceProviderMsg::SetService req = {};
	req.name = p_service;
	req.host = p_host;
	req.port = p_port;
	return JsonCodec::decode<ServiceProviderMsg::ServiceAddr>(client.sendReq(JsonCodec::encode(req)));
}

void ServiceProviderClient::stop()
{
	client.sendInd(JsonCodec::encode(ServiceProviderMsg::Stop{}));
}

}
