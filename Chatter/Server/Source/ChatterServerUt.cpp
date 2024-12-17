#include <cctype>
#include <vector>
#include <algorithm>
#include "ChatterAlfanumericGenerator.hpp"
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

struct Users
{
    struct User
    {
        bool operator==(const User& p_other) { return name == p_other.name && password == p_other.password; }
        std::string name;
        std::string password;
    };
    bool onAdd(const std::string& p_user, const std::string& p_password)
    {
        addedUsers.push_back(User{p_user, p_password});
        auto it = std::find(toBeAdded.begin(), toBeAdded.end(), User{p_user, p_password});
        return it != toBeAdded.end();
    }
    void onRemove(const std::string& p_user)
    {
        removedUsers.push_back(p_user);
    }
    bool onAuthenticate(const std::string& p_user, const std::string& p_password)
    {
        authenticateAttempts.push_back(User{p_user, p_password});
        auto it = std::find(toBeAllowedUsers.begin(), toBeAllowedUsers.end(), User{p_user, p_password});
        return it != toBeAllowedUsers.end();
    }

    void willBeRegistered(const std::string& p_user, const std::string& p_pass)
    {
        toBeAdded.push_back(User{p_user, p_pass});
    }
    void willBeAllowed(const std::string& p_user, const std::string& p_pass)
    {
        toBeAllowedUsers.push_back(User{p_user, p_pass});
    }
    bool wasAdded(const std::string& p_user, const std::string& p_pass)
    {
        return std::find(addedUsers.begin(), addedUsers.end(), User{p_user, p_pass}) != addedUsers.end();
    }
    bool wasRemoved(const std::string& p_user)
    {
        return std::find(removedUsers.begin(), removedUsers.end(), p_user) != removedUsers.end();
    }
    bool wasAuthenticated(const std::string& p_user, const std::string& p_pass)
    {
        return std::find(authenticateAttempts.begin(), authenticateAttempts.end(), User{p_user, p_pass}) != authenticateAttempts.end();
    }

    std::vector<User> addedUsers;
    std::vector<std::string> removedUsers;
    std::vector<User> authenticateAttempts;
    std::vector<User> toBeAllowedUsers;
    std::vector<User> toBeAdded;

};

class StubAuthenticator : public Chatter::IAuthenticator
{
public:
    StubAuthenticator(Users& p_users)
        : users(p_users)
    {
    }
    bool addUser(const std::string& p_user, const std::string& p_password) override
    {
        return users.onAdd(p_user, p_password);
    }
    void removeUser(const std::string& p_user) override
    {
        users.onRemove(p_user);
    }
    bool authenticate(const std::string& p_usser, const std::string& p_password) override
    {
        return users.onAuthenticate(p_usser, p_password);
    }
    Users& users;
};

TEST(generateAlfanumericStringWithGivenLength)
{
    Chatter::AlfanumericGenerator generator;
    auto result = generator.generate(50);
    IS_EQ(50, result.size());
    for(auto c : result)
    {
        IS_TRUE(std::isalnum(c));
    }
}

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

TEST(registerUserSecondTimeWillFail)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));

    users.willBeRegistered("MyUser", "Pass");
    IS_TRUE(chatter.handle(Chatter::Msg::Register{"MyUser", "Pass"}).success);
    IS_TRUE(users.wasAdded("MyUser", "Pass"));

    users.addedUsers.clear();
    IS_FALSE(chatter.handle(Chatter::Msg::Register{"MyUser", "Pass"}).success);
}

TEST(loginUnregisteredUserWillFail)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));
    IS_TRUE(chatter.handle(Chatter::Msg::Login{"MyUser", "pass"}).cookie.empty());
}

TEST(logInWillFailWhenUserIsNotAuthenticated)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));

    users.willBeRegistered("MyUser", "Pass");
    chatter.handle(Chatter::Msg::Register{"MyUser", "Pass"});

    IS_TRUE(chatter.handle(Chatter::Msg::Login{"MyUser", "pass"}).cookie.empty());
    users.wasAuthenticated("MyUser", "pass"); 
}

TEST(loginRegisteredUserWillReturnCookie)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));

    users.willBeRegistered("MyUser", "Pass");
    chatter.handle(Chatter::Msg::Register{"MyUser", "Pass"});
    users.willBeAllowed("MyUser", "Pass");
    IS_FALSE(chatter.handle(Chatter::Msg::Login{"MyUser", "Pass"}).cookie.empty());
    users.wasAuthenticated("MyUser", "Pass");
}

TEST(loginSecondTimeWillFail)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));

    users.willBeRegistered("MyUser", "Pass");
    chatter.handle(Chatter::Msg::Register{"MyUser", "Pass"});
    users.willBeAllowed("MyUser", "Pass");
    chatter.handle(Chatter::Msg::Login{"MyUser", "Pass"});
    IS_EQ("0", chatter.handle(Chatter::Msg::Login{"MyUser", "Pass"}).cookie);
}

TEST(loginAfyerLogoutWillReturnCookie)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));

    users.willBeRegistered("MyUser", "Pass");
    chatter.handle(Chatter::Msg::Register{"MyUser", "Pass"});
    users.willBeAllowed("MyUser", "Pass");
    auto cookie = chatter.handle(Chatter::Msg::Login{"MyUser", "Pass"}).cookie;
    chatter.handle(Chatter::Msg::Logout{cookie});
    IS_FALSE(chatter.handle(Chatter::Msg::Login{"MyUser", "Pass"}).cookie.empty());
}

TEST(registerUnregisteredUser)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));

    users.willBeRegistered("MyUser", "Pass");
    chatter.handle(Chatter::Msg::Register{"MyUser", "Pass"});
    users.willBeAllowed("MyUser", "Pass");
    auto cookie = chatter.handle(Chatter::Msg::Login{"MyUser", "Pass"});
    chatter.handle(Chatter::Msg::UnRegister{cookie.cookie});
    IS_TRUE(chatter.handle(Chatter::Msg::Register{"MyUser", "Pass"}).success);
}

TEST(onlineWillReturnTrue)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));

    users.willBeRegistered("MyUser", "pass");
    chatter.handle(Chatter::Msg::Register{"MyUser", "pass"});
    users.willBeAllowed("MyUser", "pass");
    auto cookie = chatter.handle(Chatter::Msg::Login{"MyUser", "pass"});
    IS_TRUE(chatter.handle(Chatter::Msg::OnLine{cookie.cookie, "127.0.0.1", "50000"}).success);
}

TEST(onlineUnregisteredUserWillReturnFalse)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));
    IS_FALSE(chatter.handle(Chatter::Msg::OnLine{"MyUser", "127.0.0.1", "50000"}).success);
}

TEST(onlineUnloggedUserWillReturnFalse)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));
    users.willBeRegistered("MyUser", "pass");
    chatter.handle(Chatter::Msg::Register{"MyUser", "pass"});
    IS_FALSE(chatter.handle(Chatter::Msg::OnLine{"MyUser", "127.0.0.1", "50000"}).success);
}

TEST(sendMessageFromUnknownUserWillFail)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));
    IS_EQ(Chatter::Msg::MessageAck::Status::UnknownUser, chatter.handle(Chatter::Msg::Message{"Sender", "Receiver", "Hello"}).status);
}

TEST(sendMessageFromUnloggedUserWillFail)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));
    users.willBeRegistered("Sender", "pass");
    chatter.handle(Chatter::Msg::Register{"Sender", "pass"});
    IS_EQ(Chatter::Msg::MessageAck::Status::UnknownUser, chatter.handle(Chatter::Msg::Message{"Sender", "Receiver", "Hello"}).status);
}

TEST(sendMessageToUnknownUserWillFail)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));
    users.willBeRegistered("Sender", "pass");
    chatter.handle(Chatter::Msg::Register{"Sender", "pass"});
    users.willBeAllowed("Sender", "pass");
    auto sender = chatter.handle(Chatter::Msg::Login{"Sender", "pass"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    IS_EQ(Chatter::Msg::MessageAck::Status::UnknownUser, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello"}).status);
}

TEST(sendMessageToNotLoggedUserWillReturnBufferdStatus)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));
    users.willBeRegistered("Sender", "pass1");
    users.willBeRegistered("Receiver", "pass2");
    chatter.handle(Chatter::Msg::Register{"Sender", "pass1"});
    chatter.handle(Chatter::Msg::Register{"Receiver", "pass2"});
    users.willBeAllowed("Sender", "pass1");
    auto sender = chatter.handle(Chatter::Msg::Login{"Sender", "pass1"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    IS_EQ(Chatter::Msg::MessageAck::Status::Buffered, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello"}).status);
}

TEST(sendMessageToOfflineUserWillReturnBufferdStatus)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));
    users.willBeRegistered("Sender", "pass1");
    users.willBeRegistered("Receiver", "pass2");
    users.willBeAllowed("Sender", "pass1");
    users.willBeAllowed("Receiver", "pass2");
    chatter.handle(Chatter::Msg::Register{"Sender", "pass1"});
    chatter.handle(Chatter::Msg::Register{"Receiver", "pass2"});
    auto sender = chatter.handle(Chatter::Msg::Login{"Sender", "pass1"});
    chatter.handle(Chatter::Msg::Login{"Receiver", "pass2"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    IS_EQ(Chatter::Msg::MessageAck::Status::Buffered, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello"}).status);
}

TEST(sendMessageToLoggedoutUserWillReturnBufferdStatus)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));
    users.willBeRegistered("Sender", "pass1");
    users.willBeRegistered("Receiver", "pass2");
    users.willBeAllowed("Sender", "pass1");
    users.willBeAllowed("Receiver", "pass2");
    chatter.handle(Chatter::Msg::Register{"Sender", "pass1"});
    chatter.handle(Chatter::Msg::Register{"Receiver", "pass2"});
    auto sender = chatter.handle(Chatter::Msg::Login{"Sender", "pass1"});
    auto receiver = chatter.handle(Chatter::Msg::Login{"Receiver", "pass2"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    chatter.handle(Chatter::Msg::OnLine{receiver.cookie, "127.0.0.1", "50001"});
    chatter.handle(Chatter::Msg::Logout{receiver.cookie});
    IS_EQ(Chatter::Msg::MessageAck::Status::Buffered, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello"}).status);
}

TEST(sendMessageToUserSwitchedToOfflineWillReturnBufferdStatus)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));
    users.willBeRegistered("Sender", "pass1");
    users.willBeRegistered("Receiver", "pass2");
    users.willBeAllowed("Sender", "pass1");
    users.willBeAllowed("Receiver", "pass2");
    chatter.handle(Chatter::Msg::Register{"Sender", "pass1"});
    chatter.handle(Chatter::Msg::Register{"Receiver", "pass2"});
    auto sender = chatter.handle(Chatter::Msg::Login{"Sender", "pass1"});
    auto receiver = chatter.handle(Chatter::Msg::Login{"Receiver", "pass2"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    chatter.handle(Chatter::Msg::OnLine{receiver.cookie, "127.0.0.1", "50001"});
    chatter.handle(Chatter::Msg::OffLine{receiver.cookie});
    IS_EQ(Chatter::Msg::MessageAck::Status::Buffered, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello"}).status);
}

TEST(sendMessage)
{
    ExchangeData data;
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));
    users.willBeRegistered("Sender", "pass1");
    users.willBeRegistered("Receiver", "pass2");
    users.willBeAllowed("Sender", "pass1");
    users.willBeAllowed("Receiver", "pass2");
    chatter.handle(Chatter::Msg::Register{"Sender", "pass1"});
    chatter.handle(Chatter::Msg::Register{"Receiver", "pass2"});
    auto receiver = chatter.handle(Chatter::Msg::Login{"Receiver", "pass2"});
    auto sender = chatter.handle(Chatter::Msg::Login{"Sender", "pass1"});
    IS_TRUE(chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"}).success);
    IS_TRUE(chatter.handle(Chatter::Msg::OnLine{receiver.cookie, "127.0.0.1", "50001"}).success);

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
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));
    users.willBeRegistered("Sender", "pass1");
    users.willBeRegistered("Receiver", "pass2");
    users.willBeAllowed("Sender", "pass1");
    users.willBeAllowed("Receiver", "pass2");
    chatter.handle(Chatter::Msg::Register{"Sender", "pass1"});
    chatter.handle(Chatter::Msg::Register{"Receiver", "pass2"});
    auto sender = chatter.handle(Chatter::Msg::Login{"Sender", "pass1"});
    auto receiver = chatter.handle(Chatter::Msg::Login{"Receiver", "pass2"});
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
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));
    users.willBeRegistered("Sender", "pass1");
    users.willBeRegistered("Receiver", "pass2");
    users.willBeAllowed("Sender", "pass1");
    users.willBeAllowed("Receiver", "pass2");
    chatter.handle(Chatter::Msg::Register{"Sender", "pass1"});
    chatter.handle(Chatter::Msg::Register{"Receiver", "pass2"});
    auto sender = chatter.handle(Chatter::Msg::Login{"Sender", "pass1"});
    auto receiver = chatter.handle(Chatter::Msg::Login{"Receiver", "pass2"});
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
    Users users;
    Chatter::Server chatter(StubConnectionFactory{data}, std::make_unique<StubAuthenticator>(users));
    users.willBeRegistered("Sender", "pass1");
    users.willBeRegistered("Receiver", "pass2");
    users.willBeAllowed("Sender", "pass1");
    users.willBeAllowed("Receiver", "pass2");
    chatter.handle(Chatter::Msg::Register{"Sender", "pass1"});
    chatter.handle(Chatter::Msg::Register{"Receiver", "pass2"});
    auto sender = chatter.handle(Chatter::Msg::Login{"Sender", "pass1"});
    auto receiver = chatter.handle(Chatter::Msg::Login{"Receiver", "pass2"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    chatter.handle(Chatter::Msg::OnLine{receiver.cookie, "127.0.0.1", "50001"});

    IS_EQ(Chatter::Msg::MessageAck::Status::Sent, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello\tWorld!!!\nHow are you?"}).status);
    IS_EQ("Hello\tWorld!!!\nHow are you?", data.sentMsg<Chatter::Msg::Message>().message);
}

int main()
{
    MAIN_START;
    
    RUN_TEST(generateAlfanumericStringWithGivenLength);
    RUN_TEST(cookieHas16AlfanumerifChars);
    RUN_TEST(genearesDifferentCookies);
    RUN_TEST(registerUserSecondTimeWillFail);
    RUN_TEST(loginUnregisteredUserWillFail);
    RUN_TEST(logInWillFailWhenUserIsNotAuthenticated);
    RUN_TEST(loginRegisteredUserWillReturnCookie);
    RUN_TEST(loginSecondTimeWillFail);
    RUN_TEST(loginAfyerLogoutWillReturnCookie);
    RUN_TEST(registerUnregisteredUser);
    RUN_TEST(onlineWillReturnTrue);
    RUN_TEST(onlineUnregisteredUserWillReturnFalse);
    RUN_TEST(onlineUnloggedUserWillReturnFalse);
    RUN_TEST(sendMessageFromUnknownUserWillFail);
    RUN_TEST(sendMessageFromUnloggedUserWillFail);
    RUN_TEST(sendMessageToUnknownUserWillFail);
    RUN_TEST(sendMessageToNotLoggedUserWillReturnBufferdStatus);
    RUN_TEST(sendMessageToOfflineUserWillReturnBufferdStatus);
    RUN_TEST(sendMessageToLoggedoutUserWillReturnBufferdStatus);
    RUN_TEST(sendMessageToUserSwitchedToOfflineWillReturnBufferdStatus);
    RUN_TEST(sendMessage);
    RUN_TEST(sendMessageToNewAddr);
    RUN_TEST(sendBufferedMessages);
    RUN_TEST(sendMessageWithSpecialChars);
    
    MAIN_END;
}
