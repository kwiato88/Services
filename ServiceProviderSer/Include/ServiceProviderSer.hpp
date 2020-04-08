#pragma once

#include "ServiceProviderSerMsg.hpp"
#include "AddrRegister.hpp"
#include "MsgService.hpp"
#include "MsgDecodingHandler.hpp"
#include "MsgReqHandler.hpp"

namespace Networking
{

typedef msg::Service<Networking::ServiceProviderMsg::ID> BaseService;
class ServiceProvider : public BaseService
{
public:
	ServiceProvider(msg::ServerFacotry p_serverFacotry);

	class SetServiceHandler : public msg::ReqHandler<ServiceProviderMsg::SetService, ServiceProviderMsg::ServiceAddr>
	{
	public:
		SetServiceHandler(AddrRegister& p_addresses);
		ServiceProviderMsg::ServiceAddr handle(const ServiceProviderMsg::SetService& p_msg);
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

	void onStopMsg();

private:
	AddrRegister addresses;
};

}
