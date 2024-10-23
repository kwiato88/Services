#pragma once

#include "ChatterMsg.hpp"
#include "ChatterCodec.hpp"
#include "MsgService.hpp"

namespace Chatter
{

using BaseService = msg::Service<Msg::Json::Codec>;
class ClientReceiver : public BaseService
{
public:
    ClientReceiver(const std::string& p_host, const std::string& p_port);

private:

};

}
