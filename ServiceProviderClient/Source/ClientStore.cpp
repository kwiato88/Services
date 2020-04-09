#include "ClientStore.hpp"
#include "MsgClient.hpp"
#include "MsgTcpIpConnection.hpp"

namespace Networking
{

msg::Client ClientStore::get(const std::string& p_serviceName)
{
	auto addr = addrs.getServiceAddr(p_serviceName);
	return msg::Client([=]() { return std::make_unique<msg::TcpIpConnection>(addr.host, addr.port); });
}

}
