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
		client.sendInd(Codec::template encode<Ind>(p_msg));
	}

	template<typename Req, typename Resp>
	Resp sendReq(const Req& p_msg)
	{
		return Codec::template decode<Resp>(client.sendReq(Codec::template encode<Req>(p_msg)));
	}
private:
	msg::Client client;
};

}
