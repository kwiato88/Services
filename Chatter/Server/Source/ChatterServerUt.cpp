#include <cctype>
#include "ChatterCookie.hpp"
#include "ChatterServer.hpp"
#include "simpleUt/SimpleUt.hpp"

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
    Chatter::Server chatter;
    IS_FALSE(chatter.handle(Chatter::Msg::Register{"MyUser"}).cookie.empty());
    IS_TRUE(chatter.handle(Chatter::Msg::Register{"MyUser"}).cookie.empty());
}

TEST(registerUnregisteredUser)
{
    Chatter::Server chatter;
    auto cookie = chatter.handle(Chatter::Msg::Register{"MyUser"});
    IS_FALSE(cookie.cookie.empty());
    chatter.handle(Chatter::Msg::UnRegister{cookie.cookie});
    IS_FALSE(chatter.handle(Chatter::Msg::Register{"MyUser"}).cookie.empty());
}

TEST(onlineWillReturnTrue)
{
    Chatter::Server chatter;
    auto cookie = chatter.handle(Chatter::Msg::Register{"MyUser"});
    IS_TRUE(chatter.handle(Chatter::Msg::OnLine{cookie.cookie, "127.0.0.1", "50000"}).success);
}

TEST(onlineUnregisteredUserWillReturnFalse)
{
    Chatter::Server chatter;
    IS_FALSE(chatter.handle(Chatter::Msg::OnLine{"MyUser", "127.0.0.1", "50000"}).success);
}

TEST(sendMessageFromUnknownUserWillFail)
{
    Chatter::Server chatter;
    IS_EQ(Chatter::Msg::MessageAck::Status::UnknownUser, chatter.handle(Chatter::Msg::Message{"Sender", "Receiver", "Hello"}).status);
}

TEST(sendMessageToUnknownUserWillFail)
{
    Chatter::Server chatter;
    auto sender = chatter.handle(Chatter::Msg::Register{"Sender"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    IS_EQ(Chatter::Msg::MessageAck::Status::UnknownUser, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello"}).status);
}

TEST(sendMessageToOfflineUserWillReturnBufferdStatus)
{
    Chatter::Server chatter;
    auto sender = chatter.handle(Chatter::Msg::Register{"Sender"});
    auto receiver = chatter.handle(Chatter::Msg::Register{"Receiver"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    IS_EQ(Chatter::Msg::MessageAck::Status::Buffered, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello"}).status);
}

TEST(sendMessageToUserSwitchedToOfflineWillReturnBufferdStatus)
{
    Chatter::Server chatter;
    auto sender = chatter.handle(Chatter::Msg::Register{"Sender"});
    auto receiver = chatter.handle(Chatter::Msg::Register{"Receiver"});
    chatter.handle(Chatter::Msg::OnLine{sender.cookie, "127.0.0.1", "50000"});
    chatter.handle(Chatter::Msg::OnLine{receiver.cookie, "127.0.0.1", "50001"});
    chatter.handle(Chatter::Msg::OffLine{receiver.cookie});
    IS_EQ(Chatter::Msg::MessageAck::Status::Buffered, chatter.handle(Chatter::Msg::Message{sender.cookie, "Receiver", "Hello"}).status);
}

//TODO: Add more tests. Probably in MT because require sending over socket
//  - send buffered messages
//  - send message to online user

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
    
    MAIN_END;
    return 0;
}
