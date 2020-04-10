#include "CachedClientStore.hpp"
#include "MsgTcpIpConnection.hpp"

namespace Networking
{

CachedClientStore::Addr::Addr(const ServiceProviderMsg::ServiceAddr& p_addr)
	: host(p_addr.host), port(p_addr.port)
{}

CachedClientStore::CachedClientStore(const std::string& p_providerHost, const std::string& p_providerPort)
	: provider(p_providerHost, p_providerPort)
{}

msg::Client CachedClientStore::get(const std::string& p_serviceName)
{
	auto addr = addrs.find(p_serviceName);
	if (addr != addrs.end() && testConnection(addr->second))
		return buildClient(addr->second);
	
	Addr newAddr{ provider.getServiceAddr(p_serviceName) };
	addrs.insert_or_assign(p_serviceName, newAddr);
	return buildClient(newAddr);
}

bool CachedClientStore::testConnection(const Addr& p_addr) const
{
	try
	{
		msg::TcpIpConnection(p_addr.host, p_addr.port);
		return true;
	}
	catch (std::exception&)
	{
		return false;
	}
}

msg::Client CachedClientStore::buildClient(const Addr& p_addr) const
{
	return msg::Client([=]() { return std::make_unique<msg::TcpIpConnection>(p_addr.host, p_addr.port); });
}

void CachedClientStore::remove(const std::string& p_serviceName)
{
	addrs.erase(p_serviceName);
}

void CachedClientStore::refresh(const std::string& p_serviceName)
{
	addrs.insert_or_assign(p_serviceName, Addr{ provider.getServiceAddr(p_serviceName) });
}

}
