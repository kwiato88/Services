#pragma once

#include <string>
#include "ClientWithCodec.hpp"
#include "ChatterCodec.hpp"
#include "ServiceProviderClient.hpp"

namespace Chatter
{

class ClientApp
{
public:
    ClientApp(const std::string& p_name);
    ~ClientApp();
    void run();

private:
    void registerAtServer();
    void unregisterAtServer();
    void goOnLine();
    void goOffLine();
    void stopReceiver();
    Msg::Message prepareMessage(const std::string& p_line);
    void sendToReceiver(Msg::Message p_msg);
    void sendToServer(Msg::Message p_msg);

    using Client = Networking::ClientWithCodec<::Chatter::Msg::Json::Codec>;
    const std::string name;
    const std::string receiverServiceName;
    std::string cookie;
    Networking::ServiceProviderClient addrs;
    Client server;
    Client receiver;
};

}
