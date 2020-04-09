#include "AddrRegister.hpp"
#include "SockListeningSocket.hpp"

#include <stdexcept>
#include <algorithm>

namespace Networking
{

bool AddrRegister::Addr::operator==(const Addr& p_other) const
{
	return host == p_other.host && port == p_other.port;
}

AddrRegister::AddrPool::AddrPool()
{
	static const unsigned int minPort = 50000;
	static const unsigned int maxPort = 51000;
	static const std::string localHost = "127.0.0.1";
	for (unsigned int i = minPort; i < maxPort; ++i)
	{
		Addr addr = {};
		addr.host = localHost;
		addr.port = std::to_string(i);
		free.push_back(addr);
	}
}

void AddrRegister::AddrPool::allocate(const Addr& p_addr)
{
	auto found = std::find(free.begin(), free.end(), p_addr);
	if (found == free.end())
		throw std::runtime_error(std::string("Host: ") + p_addr.host + ", Port: " + p_addr.port + " not available in pool");
	free.erase(found);
}
void AddrRegister::AddrPool::deallocate(const Addr& p_addr)
{
	if(std::find(free.begin(), free.end(), p_addr) == free.end())
		free.push_back(p_addr);
}
bool AddrRegister::AddrPool::isAvailable(const Addr& p_addr) const
{
	return std::find(free.begin(), free.end(), p_addr) != free.end();
}
boost::optional<AddrRegister::Addr> AddrRegister::AddrPool::findFirstFree(std::function<bool(const Addr&)> p_condition) const
{
	auto found = std::find_if(free.begin(), free.end(), p_condition);
	if (found != free.end())
		return *found;
	return boost::none;
}

AddrRegister::Addr AddrRegister::get(const std::string& p_name) const
{
	auto addr = addrs.find(p_name);
	if (addr == addrs.end())
		throw std::runtime_error(p_name + " addr not registered");
	return addr->second;
}

AddrRegister::Addr AddrRegister::add(const std::string& p_name, const Addr& p_preferendAddr)
{
	if (isAlreadyRegistered(p_name))
		throw std::runtime_error(p_name + " addr already registered");
	allocate(p_name, getAvailableAddr(p_preferendAddr));
	return get(p_name);
}

bool AddrRegister::isAlreadyRegistered(const std::string& p_name) const
{
	return addrs.find(p_name) != addrs.end();
}

bool AddrRegister::isAlreadyRegistered(const Addr& p_addr) const
{
	auto foundAddr = std::find_if(addrs.begin(), addrs.end(),
		[&](const auto& a) { return a.second == p_addr; });
	return foundAddr != addrs.end();
}

bool AddrRegister::isUsed(const Addr& p_addr) const
{
	try
	{
		sock::ListeningSocket socket;
		socket.bind(p_addr.host, p_addr.port);
		return false;
	}
	catch (std::exception&)
	{
		return true;
	}
}

boost::optional<AddrRegister::Addr> AddrRegister::tryGetPrefered(const Addr& p_preferendAddr) const
{
	if (allowedAddrs.isAvailable(p_preferendAddr))
	{
		if (!isAlreadyRegistered(p_preferendAddr) && !isUsed(p_preferendAddr))
			return p_preferendAddr;
	}
	return boost::none;
}

boost::optional<AddrRegister::Addr> AddrRegister::tryGetNotPrefered() const
{
	return allowedAddrs.findFirstFree(
		[=](const Addr& a) { return !isAlreadyRegistered(a) && !isUsed(a); });
}

AddrRegister::Addr AddrRegister::getAvailableAddr(const Addr& p_preferendAddr) const
{
	auto free = tryGetPrefered(p_preferendAddr);
	if (free)
		return *free;
	free = tryGetNotPrefered();
	if (free)
		return *free;
	throw std::runtime_error("No availalbe addr");
}

void AddrRegister::allocate(const std::string& p_name, const Addr& p_addr)
{
	allowedAddrs.allocate(p_addr);
	addrs[p_name] = p_addr;
}

void AddrRegister::remove(const std::string& p_name)
{
	if (isAlreadyRegistered(p_name))
	{
		allowedAddrs.deallocate(addrs[p_name]);
		addrs.erase(p_name);
	}
}

}
