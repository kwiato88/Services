#include "AddrRegister.hpp"
#include "SockSocketUtils.hpp"
#include "SockListeningSocket.hpp"
#include "simpleUt/SimpleUt.hpp"

TEST(getUnregisteredServiceWillThrow)
{
    Networking::AddrRegister addrs;
    THROWS(addrs.get("MyService"));
}

TEST(removeUnknownServiceWillNotThrow)
{
    Networking::AddrRegister addrs;
    addrs.remove("MyService");
}

TEST(addWithoutGivenAddrWillAssignAddr)
{
    Networking::AddrRegister addrs;
    auto addr = addrs.add("MyService", Networking::AddrRegister::Addr{});
    IS_EQ("127.0.0.1", addr.host);
    IS_FALSE(addr.port.empty());
}

TEST(addAlreadyKnownServiceWillThrow)
{
    Networking::AddrRegister addrs;
    addrs.add("MyService", Networking::AddrRegister::Addr{});
    THROWS(addrs.add("MyService", Networking::AddrRegister::Addr{}));
}

TEST(addWithGivenAddrWhenAvaliable)
{
    Networking::AddrRegister addrs;
    auto addr = addrs.add("MyService", Networking::AddrRegister::Addr{"127.0.0.1", "50044"});
    IS_EQ("127.0.0.1", addr.host);
    IS_EQ("50044", addr.port);
}

TEST(addFreeAddrWhenGivenAddrNotAvaliable)
{
    Networking::AddrRegister addrs;
    auto addr1 = addrs.add("MyService1", Networking::AddrRegister::Addr{"127.0.0.1", "49999"});
    IS_FALSE(addr1.port.empty());
    IS_NOT_EQ("49999", addr1.port);

    auto addr2 = addrs.add("MyService2", Networking::AddrRegister::Addr{"127.0.0.1", "51001"});
    IS_FALSE(addr2.port.empty());
    IS_NOT_EQ("51001", addr2.port);
}

TEST(addFreeAddrWhenRequestedAleradyAssigned)
{
    Networking::AddrRegister addrs;
    auto addr1 = addrs.add("MyService1", Networking::AddrRegister::Addr{"127.0.0.1", "50020"});
    IS_EQ("50020", addr1.port);

    auto addr2 = addrs.add("MyService2", Networking::AddrRegister::Addr{"127.0.0.1", "50020"});
    IS_FALSE(addr2.port.empty());
    IS_NOT_EQ("50020", addr2.port);
}

TEST(addRequestedAddrWhenAlreadyReleasedByOtherService)
{
    Networking::AddrRegister addrs;
    addrs.add("MyService1", Networking::AddrRegister::Addr{"127.0.0.1", "50123"});
    addrs.remove("MyService1");
    auto addr = addrs.add("MyService2", Networking::AddrRegister::Addr{"127.0.0.1", "50123"});
    IS_EQ("50123", addr.port);
}

TEST(addServiceAgainWithDifferentAddr)
{
    Networking::AddrRegister addrs;
    addrs.add("MyService", Networking::AddrRegister::Addr{"127.0.0.1", "50045"});
    addrs.remove("MyService");
    auto addr = addrs.add("MyService", Networking::AddrRegister::Addr{"127.0.0.1", "50046"});
    IS_EQ("50046", addr.port);
}

class ScopedUsedAddr
{
public:
    ScopedUsedAddr(const std::string& p_host, const std::string& p_port)
     : socket()
    {
        socket.bind(p_host, p_port);
    }
private:
    sock::ListeningSocket socket;
};

TEST(addFreeAddrWhenRequestedIsUSed)
{
    Networking::AddrRegister addrs;
    ScopedUsedAddr usedAddr("127.0.0.1", "50055");
    auto addr = addrs.add("MyService", Networking::AddrRegister::Addr{"127.0.0.1", "50055"});
    IS_FALSE(addr.port.empty());
    IS_NOT_EQ("50055", addr.port);
}

TEST(addRequestedAddrWhenNotUsedAnumore)
{
    Networking::AddrRegister addrs;
    {
        ScopedUsedAddr usedAddr("127.0.0.1", "50001");
    }
    auto addr = addrs.add("MyService", Networking::AddrRegister::Addr{"127.0.0.1", "50001"});
    IS_EQ("50001", addr.port);
}

int main()
{
    sock::init();
    MAIN_START;
    
    RUN_TEST(getUnregisteredServiceWillThrow);
    RUN_TEST(removeUnknownServiceWillNotThrow);
    RUN_TEST(addWithoutGivenAddrWillAssignAddr);
    RUN_TEST(addAlreadyKnownServiceWillThrow);
    RUN_TEST(addWithGivenAddrWhenAvaliable);
    RUN_TEST(addFreeAddrWhenGivenAddrNotAvaliable);
    RUN_TEST(addFreeAddrWhenRequestedAleradyAssigned);
    RUN_TEST(addRequestedAddrWhenAlreadyReleasedByOtherService);
    RUN_TEST(addServiceAgainWithDifferentAddr);
    RUN_TEST(addFreeAddrWhenRequestedIsUSed);
    RUN_TEST(addRequestedAddrWhenNotUsedAnumore);
    
    MAIN_END;
    sock::cleanup();
    return 0;
}
