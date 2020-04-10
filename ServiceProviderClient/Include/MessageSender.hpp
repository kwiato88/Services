#pragma once

#include "ClientWithCodec.hpp"
#include "ClientStore.hpp"
#include "MsgTcpIpConnection.hpp"

namespace Networking
{

class MessageSender
{
public:
	MessageSender(const std::string& p_providerHost = "127.0.0.1", const std::string& p_providerPort = "50000")
		: clients(p_providerHost, p_providerPort)
	{}

	template<typename Codec, typename Ind>
	void sendInd(const std::string& p_service, const Ind& p_msg)
	{
		clients.getWithCodec<Codec>(p_service).sendInd<Ind>(p_msg);
	}

	template<typename Codec, typename Req, typename Resp>
	Resp sendReq(const std::string& p_service, const Req& p_msg)
	{
		return clients.getWithCodec<Codec>(p_service).sendReq<Req, Resp>(p_msg);
	}

	void sendInd(const std::string& p_service, const std::string& p_msg)
	{
		clients.get(p_service).sendInd(p_msg);
	}

	std::string sendReq(const std::string& p_service, const std::string& p_msg)
	{
		return clients.get(p_service).sendReq(p_msg);
	}

	template<typename Codec, typename Ind>
	void sendInd(const std::string& p_host, const std::string& p_port, const Ind& p_msg)
	{
		ClientWithCodec<Codec> client(msg::Client([=]() { return std::make_unique<msg::TcpIpConnection>(p_host, p_port); }));
		client.sendInd<Ind>(p_msg);
	}

	template<typename Codec, typename Req, typename Resp>
	Resp sendReq(const std::string& p_host, const std::string& p_port, const Req& p_msg)
	{
		ClientWithCodec<Codec> client(msg::Client([=]() { return std::make_unique<msg::TcpIpConnection>(p_host, p_port); }));
		return client.sendReq<Req, Resp>(p_msg);
	}

	void sendInd(const std::string& p_host, const std::string& p_port, const std::string& p_msg)
	{
		msg::Client client([=]() { return std::make_unique<msg::TcpIpConnection>(p_host, p_port); });
		client.sendInd(p_msg);
	}

	std::string sendReq(const std::string& p_host, const std::string& p_port, const std::string& p_msg)
	{
		msg::Client client([=]() { return std::make_unique<msg::TcpIpConnection>(p_host, p_port); });
		return client.sendReq(p_msg);
	}
private:
	ClientStore clients;
};

}
