#pragma once

#include <string>
#include <vector>
#include <memory>
#include "ClientWithCodec.hpp"
#include "ChatterCodec.hpp"
#include "ServiceProviderClient.hpp"
#include "WinApiThread.hpp"
#include "WinApiMutex.hpp"

namespace Chatter
{

class ClientApp
{
public:
    ClientApp(const std::string& p_name, std::function<void()> p_onMessageReceived);
    ~ClientApp();
    std::string goOnLine();
    void goOffLine();
    void sendMessage(const std::string& p_message);
    std::vector<std::string> getChats() const;
    std::string getChatWith(std::size_t p_idx) const;
    std::string getCurrentChat() const;
    std::size_t getCurrentChatIdx() const;
    void chatWith(std::size_t p_idx);
    std::size_t newChat(const std::string& p_name);
    void removeChat(std::size_t p_idx);
    void receivedMessage(const Msg::Message& p_message);

private:
    using Client = Networking::ClientWithCodec<::Chatter::Msg::Json::Codec>;
    using ReceivedMessageCallback = std::function<void(const Msg::Message&)>;
    class Receiver
    {
    public:
        Receiver(const std::string& p_name, const std::string& p_cookie,
            const std::string& p_host, const std::string& p_port,
            const std::string& p_serverHost, const std::string& p_serverPort,
            ReceivedMessageCallback p_onMessageReceived);
        ~Receiver();
    private:
        void notifyServerOffLine();
        void stopReceiver();
        void receiverMain();

        Client api;
        WinApi::Thread thread;
        Client serverApi;
        std::string host;
        std::string port;
        std::string cookie;
        ReceivedMessageCallback onMessageReceived;
    };
    class Chat
    {
    public:
        Chat(const std::string& p_withUser = "");
        Chat(const Chat&) = default;
        Chat(Chat&&) = default;
        Chat& operator=(const Chat&) = default;
        Chat& operator=(Chat&&) = default;
        std::string toString() const;
        std::string with() const;
        std::string header() const;
        void add(const std::string& p_user, const std::string& p_message);
        void read();
    private:
        std::string withUser;
        std::vector<std::string> messages;
        std::size_t unreadCount;
    };

    void registerAtServer();
    void unregisterAtServer();
    std::size_t findChatWith(const std::string& p_name) const;
    void sendToServer(const std::string& p_message);
    std::size_t newChatWithoutLock(const std::string& p_name);
    
    const std::string name;
    const std::string receiverServiceName;
    std::unique_ptr<Receiver> receiver;
    std::string cookie;
    Networking::ServiceProviderClient addrs;
    Client server;
    Networking::ServiceProviderMsg::ServiceAddr serverAddr;
    std::vector<Chat> chats;
    std::size_t activeChat;
    std::function<void()> onMessageReceived;
    mutable WinApi::Mutex chatsLock;
};

}
