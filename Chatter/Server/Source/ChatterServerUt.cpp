#include <cctype>
#include <vector>
#include "ChatterCookie.hpp"
#include "ChatterServer.hpp"
#include "ChatterCodec.hpp"
#include "MsgConnection.hpp"
#include "simpleUt/SimpleUt.hpp"

class ExchangeData
{
public:
    void onSend(const std::string& p_data)
    {
        sent.push_back(p_data);
    }
    std::string onReceive() const
    {
        return received;
    }
    void onConnect(const std::string& p_host, const std::string& p_port)
    {
        host = p_host;
        port = p_port;
    }
    std::size_t numberOfSentMessages() const
    {
        return sent.size();
    }
    template<typename T>
    T sentMsg(std::size_t p_index = 0) const
    {
        return Chatter::Msg::Json::Codec::decode<T>(sent.at(p_index));
    }
    std::string connectedToHost() const
    {
        return host;
    }
    std::string connectedToPort() const
    {
        return port;
    }
private:
    std::vector<std::string> sent;
    std::string received;
    std::string host;
    std::string port;
};

class StubConnection : public msg::Connection
{
public:
    StubConnection(ExchangeData& p_data)
        : data(p_data)
    {
    }
    void send(const std::string& p_data) override
    {
        data.onSend(p_data);
    }
    
    std::string receive() override
    {
        return data.onReceive();
    }
    ExchangeData& data;
};

struct StubConnectionFactory
{
    StubConnectionFactory(ExchangeData& p_data)
        : data(p_data)
    {
    }
    std::unique_ptr<msg::Connection> operator()(const std::string& host, const std::string& port)
    {
        data.onConnect(host, port);
        return std::make_unique<StubConnection>(data);
    }
    ExchangeData& data;
};

TEST(cookieHas16AlfanumerifChars)
{
    Chatter::CookieStore cookies;
    auto cookie = cookies.allocateCookie();
    IS_EQ(16, cookie.toString().size());
    for(auto c : cookie.toString())
    {
        IS_TRUE(std::isalnum(c));
    }
}

TEST(genearesDifferentCookies)
{
    Chatter::CookieStore cookies;
    auto cookie1 = cookies.allocateCookie();
    auto cookie2 = cookies.allocateCookie();
    auto cookie3 = cookies.allocateCookie();
    IS_NOT_EQ(cookie1.toString(), cookie2.toString());
    IS_NOT_EQ(cookie1.toString(), cookie3.toString());
    IS_NOT_EQ(cookie2.toString(), cookie3.toString());
}

TEST(registerUserSecondTimeWillReturnEmptyCookie)
{
    ExchangeData data;
    Chatter::Server chatter(StubConnectionFactory{data});
    IS_FALSE(chatter.handle(Chatter::Msg::Register{"MyUser"}).cookie.empty());
    IS_TRUE(chatter.handle(Chatter::Msg::Register{"MyUser"}).cookie.empty());
}

TEST(registerUnregisteredUser)
{
    ExchangeData data;
    Chatter::Server chatter(StubConnectionFactory{data});
    auto cookie = chatter.handle(Chatter::Msg::Register{"MyUser"});
    IS_FALSE(cookie.cookie.empty());
    chatter.handle(Chatter::Msg::UnRegister{cookie.cookie});
    IS_FALSE(chatter.handle(Chatter::Msg::Register{"MyUser"}).cookie.empty());
}

TEST(onlineWillReturnTrue)
{
    ExchangeData data;
    Chatter::Server chatter(StubConnectionFactory{data});
    auto cookie = chatter.handle(Chatter::Msg::Register{"MyUser"});
    IS_TRUE(chatter.handle(Chatter::Msg::OnLine{cookie.cookie, "127.0.0.1", "50000"}).success);
}

TEST(onlineUnregisteredUserWillReturnFalse)
{
    ExchangeData data;
    Chatter::Server chatter(StubConnectionFactory{data});
    IS_FALSE(chatter.handle(Chatter::Msg::OnLine{"MyUser", "127.0.0.1", "50000"}).success);
}

TEST(sendMessageFromUnknownUserWillFail)
{
    ExchangeData data;
    Chatter::Server chatter(StubConnectionFactory{data});
    IS_EQ(Chatter::Msg::MessageAck::Status::UnknownUser, chatter.handle(Chatter::Msg::Message{"Sender", "Receiver", "Hello"}).status);
}

TEST(sendMessageToUnknownUserWillFail)
{
    ExchangeData data;
    Chatter::Server chatter(StubConnectionFactory{data});
    auto sender = chatter.handle(Chatter::Msg::Register{"Sender"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    IS_EQ(Chatter::Msg::MessageAck::Status::UnknownUser, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello"}).status);
}

TEST(sendMessageToOfflineUserWillReturnBufferdStatus)
{
    ExchangeData data;
    Chatter::Server chatter(StubConnectionFactory{data});
    auto sender = chatter.handle(Chatter::Msg::Register{"Sender"});
    auto receiver = chatter.handle(Chatter::Msg::Register{"Receiver"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    IS_EQ(Chatter::Msg::MessageAck::Status::Buffered, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello"}).status);
}

TEST(sendMessageToUserSwitchedToOfflineWillReturnBufferdStatus)
{
    ExchangeData data;
    Chatter::Server chatter(StubConnectionFactory{data});
    auto sender = chatter.handle(Chatter::Msg::Register{"Sender"});
    auto receiver = chatter.handle(Chatter::Msg::Register{"Receiver"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    chatter.handle(Chatter::Msg::OnLine{receiver.cookie, "127.0.0.1", "50001"});
    chatter.handle(Chatter::Msg::OffLine{receiver.cookie});
    IS_EQ(Chatter::Msg::MessageAck::Status::Buffered, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello"}).status);
}

TEST(sendMessage)
{
    ExchangeData data;
    Chatter::Server chatter(StubConnectionFactory{data});
    auto sender = chatter.handle(Chatter::Msg::Register{"Sender"});
    auto receiver = chatter.handle(Chatter::Msg::Register{"Receiver"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    chatter.handle(Chatter::Msg::OnLine{receiver.cookie, "127.0.0.1", "50001"});

    IS_EQ(Chatter::Msg::MessageAck::Status::Sent, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello"}).status);
    IS_EQ("127.0.0.1", data.connectedToHost());
    IS_EQ("50001", data.connectedToPort());
    Chatter::Msg::Message sentToReceiver = data.sentMsg<Chatter::Msg::Message>();
    IS_EQ("Hello", sentToReceiver.message);
    IS_EQ("Receiver", sentToReceiver.to);
    IS_EQ("Sender", sentToReceiver.from);
}

TEST(sendMessageToNewAddr)
{
    ExchangeData data;
    Chatter::Server chatter(StubConnectionFactory{data});
    auto sender = chatter.handle(Chatter::Msg::Register{"Sender"});
    auto receiver = chatter.handle(Chatter::Msg::Register{"Receiver"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    chatter.handle(Chatter::Msg::OnLine{receiver.cookie, "127.0.0.1", "50001"});
    chatter.handle(Chatter::Msg::OffLine{receiver.cookie});
    chatter.handle(Chatter::Msg::OnLine{receiver.cookie, "127.0.0.1", "50002"});

    IS_EQ(Chatter::Msg::MessageAck::Status::Sent, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello"}).status);
    IS_EQ("50002", data.connectedToPort());
}

TEST(sendBufferedMessages)
{
    ExchangeData data;
    Chatter::Server chatter(StubConnectionFactory{data});
    auto sender = chatter.handle(Chatter::Msg::Register{"Sender"});
    auto receiver = chatter.handle(Chatter::Msg::Register{"Receiver"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    
    IS_EQ(Chatter::Msg::MessageAck::Status::Buffered, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Message 1"}).status);
    IS_EQ(Chatter::Msg::MessageAck::Status::Buffered, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Message 2"}).status);
    chatter.handle(Chatter::Msg::OnLine{receiver.cookie, "127.0.0.1", "50001"});
    IS_EQ(2, data.numberOfSentMessages());
    auto sentMsg = data.sentMsg<Chatter::Msg::Message>(0);
    IS_EQ("Message 1", sentMsg.message);
    IS_EQ("Receiver", sentMsg.to);
    IS_EQ("Sender", sentMsg.from);
    sentMsg = data.sentMsg<Chatter::Msg::Message>(1);
    IS_EQ("Message 2", sentMsg.message);
    IS_EQ("Receiver", sentMsg.to);
    IS_EQ("Sender", sentMsg.from);
}

TEST(sendMessageWithSpecialChars)
{
    ExchangeData data;
    Chatter::Server chatter(StubConnectionFactory{data});
    auto sender = chatter.handle(Chatter::Msg::Register{"Sender"});
    auto receiver = chatter.handle(Chatter::Msg::Register{"Receiver"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    chatter.handle(Chatter::Msg::OnLine{receiver.cookie, "127.0.0.1", "50001"});

    IS_EQ(Chatter::Msg::MessageAck::Status::Sent, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello\tWorld!!!\nHow are you?"}).status);
    IS_EQ("Hello\tWorld!!!\nHow are you?", data.sentMsg<Chatter::Msg::Message>().message);
}

int main()
{
    MAIN_START;
    
    RUN_TEST(cookieHas16AlfanumerifChars);
    RUN_TEST(genearesDifferentCookies);
    RUN_TEST(registerUserSecondTimeWillReturnEmptyCookie);
    RUN_TEST(registerUnregisteredUser);
    RUN_TEST(onlineWillReturnTrue);
    RUN_TEST(onlineUnregisteredUserWillReturnFalse);
    RUN_TEST(sendMessageFromUnknownUserWillFail);
    RUN_TEST(sendMessageToUnknownUserWillFail);
    RUN_TEST(sendMessageToOfflineUserWillReturnBufferdStatus);
    RUN_TEST(sendMessageToUserSwitchedToOfflineWillReturnBufferdStatus);
    RUN_TEST(sendMessage);
    RUN_TEST(sendMessageToNewAddr);
    RUN_TEST(sendBufferedMessages);
    RUN_TEST(sendMessageWithSpecialChars);
    
    MAIN_END;
}
