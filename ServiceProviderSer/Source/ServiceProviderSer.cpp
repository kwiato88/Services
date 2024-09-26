#include <iostream>
#include "ServiceProviderSer.hpp"
#include "ServiceProviderSerCodec.hpp"
#include "MsgReqHandler.hpp"

namespace Networking
{

class LoggingHandler : public msg::Handler
{
public:
	std::string handle(const std::string&)
	{
		std::cerr << "Message ignored" << std::endl;
		return "";
	}
};

class SetServiceHandler : public msg::ReqHandler<ServiceProviderMsg::SetService, ServiceProviderMsg::ServiceAddr>
{
public:
	SetServiceHandler(AddrRegister& p_addresses);
	ServiceProviderMsg::ServiceAddr handle(const ServiceProviderMsg::SetService& p_msg);
private:
	AddrRegister& addresses;
};

class RemoveServiceHandler : public msg::IndHandler<ServiceProviderMsg::RemoveService>
{
public:
	RemoveServiceHandler(AddrRegister& p_addresses);
	void handle(const ServiceProviderMsg::RemoveService& p_msg);
private:
	AddrRegister& addresses;
};

class GetServiceHandler : public msg::ReqHandler<ServiceProviderMsg::GetServiceAddr, ServiceProviderMsg::ServiceAddr>
{
public:
	GetServiceHandler(AddrRegister& p_addresses);
	ServiceProviderMsg::ServiceAddr handle(const ServiceProviderMsg::GetServiceAddr& p_msg);
private:
	AddrRegister& addresses;
};

SetServiceHandler::SetServiceHandler(AddrRegister& p_addresses)
	: addresses(p_addresses)
{}
ServiceProviderMsg::ServiceAddr SetServiceHandler::handle(const ServiceProviderMsg::SetService& p_msg)
{
	std::cout << "Try to register " << p_msg.name << " service at [" << p_msg.host << ":" << p_msg.port << "]" << std::endl;

	auto addr = addresses.add(p_msg.name, AddrRegister::Addr{ p_msg.host, p_msg.port });
	ServiceProviderMsg::ServiceAddr resp = {};
	resp.host = addr.host;
	resp.port = addr.port;

	std::cout << p_msg.name << " service registered at [" << resp.host << ":" << resp.port << "]" << std::endl;
	return resp;
}

RemoveServiceHandler::RemoveServiceHandler(AddrRegister& p_addresses)
	: addresses(p_addresses)
{}
void RemoveServiceHandler::handle(const ServiceProviderMsg::RemoveService& p_msg)
{
	std::cout << "Try to release " << p_msg.name << " service" << std::endl;
	addresses.remove(p_msg.name);
}

GetServiceHandler::GetServiceHandler(AddrRegister& p_addresses)
	: addresses(p_addresses)
{}
ServiceProviderMsg::ServiceAddr GetServiceHandler::handle(const ServiceProviderMsg::GetServiceAddr& p_msg)
{
	std::cout << "Get " << p_msg.name << " service addr" << std::endl;

	auto addr = addresses.get(p_msg.name);
	ServiceProviderMsg::ServiceAddr resp = {};
	resp.host = addr.host;
	resp.port = addr.port;

	return resp;
}

void ServiceProvider::onStopMsg()
{
	std::cout << "Stop service addrs provider" << std::endl;
}

ServiceProvider::ServiceProvider(msg::ServerFacotry p_serverFacotry)
	: BaseService(p_serverFacotry)
{
	//TODO: stop handler
	//addIndHandler<StopHandler, JsonCodec>(std::make_unique<StopHandler>(*this, std::bind(&ServiceProvider::onStopMsg, this)));
	addHandler<ServiceProviderMsg::SetService, ServiceProviderMsg::ServiceAddr>(
		ServiceProviderMsg::SetService::id, std::make_shared<SetServiceHandler>(addresses));
	addHandler<ServiceProviderMsg::GetServiceAddr, ServiceProviderMsg::ServiceAddr>(
		ServiceProviderMsg::GetServiceAddr::id, std::make_shared<GetServiceHandler>(addresses));
	addHandler<ServiceProviderMsg::RemoveService>(
		ServiceProviderMsg::RemoveService::id, std::make_shared<RemoveServiceHandler>(addresses));
	setDefaultHandler(std::make_shared<LoggingHandler>());
}

}
