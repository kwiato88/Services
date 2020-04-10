#pragma once

#include "ServiceProviderSerMsg.hpp"
#include "AddrRegister.hpp"
#include "MsgService.hpp"

namespace Networking
{

typedef msg::Service<Networking::ServiceProviderMsg::ID> BaseService;
class ServiceProvider : public BaseService
{
public:
	ServiceProvider(msg::ServerFacotry p_serverFacotry);

private:
	void onStopMsg();
	AddrRegister addresses;
};

}
