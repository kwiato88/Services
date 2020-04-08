#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <boost/optional.hpp>

namespace Networking
{

class AddrRegister
{
public:
	struct Addr
	{
		std::string host;
		std::string port;
		
		bool operator==(const Addr& p_other) const;
	};

	Addr get(const std::string& p_name) const;
	Addr add(const std::string& p_name, const Addr& p_preferendAddr);

private:
	class AddrPool
	{
	public:
		AddrPool();
		void allocate(const Addr& p_addr);
		void deallocate(const Addr& p_addr);
		bool isAvailable(const Addr& p_addr) const;
		boost::optional<Addr> findFirstFree(std::function<bool(const Addr&)> p_condition) const;
	private:
		std::vector<Addr> free;
	};

	Addr getAvailableAddr(const Addr& p_preferendAddr) const;
	boost::optional<Addr> tryGetPrefered(const Addr& p_preferendAddr) const;
	boost::optional<Addr> tryGetNotPrefered() const;
	bool isAlreadyRegistered(const Addr& p_addr) const;
	bool isUsed(const Addr& p_addr) const;
	void allocate(const std::string& p_name, const Addr& p_addr);

	std::map<std::string, Addr> addrs;
	AddrPool allowedAddrs;
};

}
