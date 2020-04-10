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
	void onError(std::exception& e);
private:
	AddrRegister& addresses;
};

class RemoveServiceHandler : public msg::IndicationHandler<ServiceProviderMsg::RemoveService>
{
public:
	RemoveServiceHandler(AddrRegister& p_addresses);
	void handle(const ServiceProviderMsg::RemoveService& p_msg);
	void onError(std::exception& e);
private:
	AddrRegister& addresses;
};

class GetServiceHandler : public msg::ReqHandler<ServiceProviderMsg::GetServiceAddr, ServiceProviderMsg::ServiceAddr>
{
public:
	GetServiceHandler(AddrRegister& p_addresses);
	ServiceProviderMsg::ServiceAddr handle(const ServiceProviderMsg::GetServiceAddr& p_msg);
	void onError(std::exception& e);
private:
	AddrRegister& addresses;
};

SetServiceHandler::SetServiceHandler(AddrRegister& p_addresses)
	: addresses(p_addresses)
{}
ServiceProviderMsg::ServiceAddr  SetServiceHandler::handle(const ServiceProviderMsg::SetService& p_msg)
{
	std::cout << "Try to register " << p_msg.name << " service at [" << p_msg.host << ":" << p_msg.port << "]" << std::endl;

	auto addr = addresses.add(p_msg.name, AddrRegister::Addr{ p_msg.host, p_msg.port });
	ServiceProviderMsg::ServiceAddr resp = {};
	resp.host = addr.host;
	resp.port = addr.port;

	std::cout << p_msg.name << " service registered at [" << resp.host << ":" << resp.port << "]" << std::endl;
	return resp;
}
void SetServiceHandler::onError(std::exception& e)
{
	std::cerr << "Failed to set address. " << e.what() << std::endl;
}

RemoveServiceHandler::RemoveServiceHandler(AddrRegister& p_addresses)
	: addresses(p_addresses)
{}
void RemoveServiceHandler::handle(const ServiceProviderMsg::RemoveService& p_msg)
{
	std::cout << "Try to release " << p_msg.name << " service" << std::endl;
	addresses.remove(p_msg.name);
}
void RemoveServiceHandler::onError(std::exception& e)
{
	std::cerr << "Failed to remove service. " << e.what() << std::endl;
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
void GetServiceHandler::onError(std::exception& e)
{
	std::cerr << "Failed to get address. " << e.what() << std::endl;
}

void ServiceProvider::onStopMsg()
{
	std::cout << "Stop service addrs provider" << std::endl;
}

ServiceProvider::ServiceProvider(msg::ServerFacotry p_serverFacotry)
	: BaseService(p_serverFacotry, &Networking::ServiceProviderMsg::Json::Codec::getId)
{
	using JsonCodec = ServiceProviderMsg::Json::Codec;
	using StopHandler = BaseService::StopHandler<ServiceProviderMsg::Stop>;
	addIndHandler<StopHandler, JsonCodec>(std::make_unique<StopHandler>(*this, std::bind(&ServiceProvider::onStopMsg, this)));
	addReqHandler<SetServiceHandler, JsonCodec>(std::make_unique<SetServiceHandler>(addresses));
	addReqHandler<GetServiceHandler, JsonCodec>(std::make_unique<GetServiceHandler>(addresses));
	addIndHandler<RemoveServiceHandler, JsonCodec>(std::make_unique<RemoveServiceHandler>(addresses));
	setDefaultHandler(std::make_unique<LoggingHandler>());
}

}
