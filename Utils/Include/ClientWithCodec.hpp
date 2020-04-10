#pragma once

#include "MsgClient.hpp"

namespace Networking
{

template<typename Codec>
class ClientWithCodec
{
public:
	ClientWithCodec(const msg::Client& p_client) : client(p_client) {}

	template<typename Ind>
	void sendInd(const Ind& p_msg)
	{
		client.sendInd(Codec::encode<Ind>(p_msg));
	}
	template<>
	void sendInd<std::string>(const std::string& p_msg)
	{
		client.sendInd(p_msg);
	}

	template<typename Req, typename Resp>
	Resp sendReq(const Req& p_msg)
	{
		return Codec::decode<Resp>(client.sendReq(Codec::encode<Req>(p_msg)));
	}
	template<>
	std::string sendReq<std::string, std::string>(const std::string& p_msg)
	{
		return client.sendReq(p_msg);
	}
private:
	msg::Client client;
};

}
