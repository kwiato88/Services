#include "ClientStore.hpp"
#include "MsgClient.hpp"
#include "MsgTcpIpConnection.hpp"

namespace Networking
{

ClientStore::ClientStore(const std::string& p_providerHost, const std::string& p_providerPort)
	:addrs(p_providerHost, p_providerPort)
{
}

msg::Client ClientStore::get(const std::string& p_serviceName)
{
	auto addr = addrs.getServiceAddr(p_serviceName);
	return msg::Client([=]() { return std::make_unique<msg::TcpIpConnection>(addr.host, addr.port); });
}

}
