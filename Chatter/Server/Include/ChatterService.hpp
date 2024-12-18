#pragma once

#include <memory>
#include <filesystem>
#include "ChatterMsg.hpp"
#include "ChatterCodec.hpp"
#include "ChatterServer.hpp"
#include "MsgService.hpp"

namespace Chatter
{
using BaseService = msg::Service<Msg::Json::Codec>;
class Service : public BaseService
{
public:
    Service(const std::filesystem::path& p_configDir);
    ~Service();

private:
    std::unique_ptr<msg::Server> createServer();
    void setup();
    template <typename Req, typename Resp>
    void add()
    {
        addHandler<Req, Resp>(Req::id, chatter);
    }
    template <typename Ind>
    void add()
    {
        addHandler<Ind>(Ind::id, chatter);
    }

    std::string host;
    std::string port;
    const std::string name = "ChatterService";
    std::shared_ptr<Server> chatter;
};

}
