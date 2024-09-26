#pragma once

#include "ServiceProviderSerMsg.hpp"
#include "ServiceProviderSerCodec.hpp"
#include "AddrRegister.hpp"
#include "MsgService.hpp"

namespace Networking
{

typedef msg::Service<ServiceProviderMsg::Json::Codec> BaseService;
class ServiceProvider : public BaseService
{
public:
	ServiceProvider(msg::ServerFacotry p_serverFacotry);

private:
	void onStopMsg();
	AddrRegister addresses;
};

}
