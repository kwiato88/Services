#include <iostream>

#include "SockSocketUtils.hpp"
#include "ServiceProviderClient.hpp"
#include "simpleUt/SimpleUt.hpp"

TEST(getUnregisteredServiceShouldFail)
{
	Networking::ServiceProviderClient provider;
	THROWS(provider.getServiceAddr("MyService"));
}

TEST(shouldAssignDifferentAddrWhenRequestedIsnotAvailable)
{
	Networking::ServiceProviderClient provider;
	auto assigned = provider.setServiceAddr("MyService", "127.0.0.1", "1111");
	auto actualAddr = provider.getServiceAddr("MyService");
	IS_NOT_EQ(assigned.host, "");
	IS_EQ(assigned.host, actualAddr.host);
	IS_EQ(assigned.port, actualAddr.port);

	std::cout << "MyService assigned to [" << assigned.host << ":" << assigned.port << "]" << std::endl;
}

TEST(registerAlreadyRegisteredServiceShouldFail)
{
	Networking::ServiceProviderClient provider;
	THROWS(provider.setServiceAddr("MyService", "127.0.0.1", "50005"));
}

TEST(shouldAssignDifferentAddrWhenRequestedAlreadyAssigned)
{
	Networking::ServiceProviderClient provider;
	auto assigned = provider.setServiceAddr("MyService2", "127.0.0.1", "50001");
	auto actualAddr = provider.getServiceAddr("MyService2");
	IS_NOT_EQ(assigned.host, "50001");
	IS_EQ(assigned.host, actualAddr.host);
	IS_EQ(assigned.port, actualAddr.port);
}

TEST(shouldAssignDifferentAddrWhenRequestedAlreadyUsed)
{
	Networking::ServiceProviderClient provider;
	auto assigned = provider.setServiceAddr("MyService3", "127.0.0.1", "50000");
	auto actualAddr = provider.getServiceAddr("MyService3");
	IS_NOT_EQ(assigned.host, "50000");
	IS_EQ(assigned.host, actualAddr.host);
	IS_EQ(assigned.port, actualAddr.port);
}

TEST(registerServiceOnFreeAddrShouldSucceed)
{
	Networking::ServiceProviderClient provider;
	auto assigned = provider.setServiceAddr("MyService4", "127.0.0.1", "50100");
	auto actualAddr = provider.getServiceAddr("MyService4");
	IS_EQ(assigned.port, "50100");
	IS_EQ(assigned.host, actualAddr.host);
	IS_EQ(assigned.port, actualAddr.port);
}

TEST(registerServiceWithoutPreferredAddrShouldSucceed)
{
	Networking::ServiceProviderClient provider;
	auto assigned = provider.setServiceAddr("MyService5", "", "");
	auto actualAddr = provider.getServiceAddr("MyService5");
	IS_NOT_EQ(assigned.host, "");
	IS_NOT_EQ(assigned.port, "");
	IS_EQ(assigned.host, actualAddr.host);
	IS_EQ(assigned.port, actualAddr.port);
}

TEST(shouldRemoveService)
{
	Networking::ServiceProviderClient provider;
	provider.removeServiceAddr("MyService");
	THROWS(provider.getServiceAddr("MyService"));
}

TEST(removeNotExistingServiceShouldNotThrow)
{
	Networking::ServiceProviderClient provider;
	provider.removeServiceAddr("MyService10");
}

TEST(removeAllServices)
{
	Networking::ServiceProviderClient provider;
	provider.removeServiceAddr("MyService2");
	provider.removeServiceAddr("MyService3");
	provider.removeServiceAddr("MyService4");
	provider.removeServiceAddr("MyService5");
}

TEST(stopProvider)
{
	Networking::ServiceProviderClient provider;
	provider.stop();
}

int main()
{
	MAIN_START;
	sock::init();

	RUN_TEST(getUnregisteredServiceShouldFail);
	RUN_TEST(shouldAssignDifferentAddrWhenRequestedIsnotAvailable);
	RUN_TEST(registerAlreadyRegisteredServiceShouldFail);
	RUN_TEST(shouldAssignDifferentAddrWhenRequestedAlreadyAssigned);
	RUN_TEST(shouldAssignDifferentAddrWhenRequestedAlreadyUsed);
	RUN_TEST(registerServiceOnFreeAddrShouldSucceed);
	RUN_TEST(registerServiceWithoutPreferredAddrShouldSucceed);
	RUN_TEST(shouldRemoveService);
	RUN_TEST(removeNotExistingServiceShouldNotThrow);
	RUN_TEST(removeAllServices);
	RUN_TEST(stopProvider);

	sock::cleanup();
	MAIN_END;
	return 0;
}
