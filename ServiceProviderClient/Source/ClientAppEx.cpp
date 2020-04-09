#include <iostream>

#include "SockSocketUtils.hpp"
#include "ServiceProviderClient.hpp"

void setSer(Networking::ServiceProviderClient& p_provider, const std::string& p_name, const std::string& p_host, const std::string& p_port)
{
	try
	{
		std::cout << "\nRegister " << p_name << " at [" << p_host << ":" << p_port << "]" << std::endl;
		auto resp = p_provider.setServiceAddr(p_name, p_host, p_port);
		std::cout << "Registered " << p_name << " at [" << resp.host << ":" << resp.port << "]" << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << "ERROR: " << e.what() << std::endl;
	}
}
void getSer(Networking::ServiceProviderClient& p_provider, const std::string& p_name)
{
	try
	{
		std::cout << "\nGet " << p_name << " addr" << std::endl;
		auto resp = p_provider.getServiceAddr(p_name);
		std::cout << "Received " << p_name << " at [" << resp.host << ":" << resp.port << "]" << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << "ERROR: " << e.what() << std::endl;
	}
}

int main()
{
	try
	{
		sock::init();

		Networking::ServiceProviderClient serviceProvider;

		std::cout << "\n\n    Try to get unregistered service" << std::endl;
		getSer(serviceProvider, "MyService");

		std::cout << "\n\n    Try to register service on non available addr" << std::endl;
		setSer(serviceProvider, "MyService", "127.0.0.1", "1111");
		getSer(serviceProvider, "MyService");

		std::cout << "\n\n    Try to register already registered service" << std::endl;
		setSer(serviceProvider, "MyService", "127.0.0.1", "50005");
		getSer(serviceProvider, "MyService");

		std::cout << "\n\n    Try to register service on already registered addr" << std::endl;
		setSer(serviceProvider, "MyService2", "127.0.0.1", "50001");
		getSer(serviceProvider, "MyService2");

		std::cout << "\n\n    Try to register service on used addr" << std::endl;
		setSer(serviceProvider, "MyService3", "127.0.0.1", "50000");
		getSer(serviceProvider, "MyService3");

		std::cout << "\n\n    Try to register service on free addr" << std::endl;
		setSer(serviceProvider, "MyService4", "127.0.0.1", "50100");
		getSer(serviceProvider, "MyService4");

		std::cout << "\n\n    Try to register service without giving addr" << std::endl;
		setSer(serviceProvider, "MyService5", "", "");
		getSer(serviceProvider, "MyService5");

		std::cout << "\n\nRemove MyService services" << std::endl;
		getSer(serviceProvider, "MyService");
		std::cout << "\nRemove MyService" << std::endl;
		serviceProvider.removeServiceAddr("MyService");
		getSer(serviceProvider, "MyService");

		std::cout << "\n\nTry remove not existing services" << std::endl;
		getSer(serviceProvider, "MyService10");
		std::cout << "Remove MyService10" << std::endl;
		serviceProvider.removeServiceAddr("MyService10");
		getSer(serviceProvider, "MyService10");

		std::cout << "\n\nRemove all services" << std::endl;
		serviceProvider.removeServiceAddr("MyService2");
		serviceProvider.removeServiceAddr("MyService3");
		serviceProvider.removeServiceAddr("MyService4");
		serviceProvider.removeServiceAddr("MyService5");

		std::cout << "\n\nStop service provider" << std::endl;
		serviceProvider.stop();

		std::cout << "Finished" << std::endl;
		sock::cleanup();
		return 0;
	}
	catch (std::exception& e)
	{
		std::cerr << "Something went wrong. Exception: " << e.what() << std::endl;
		sock::cleanup();
		return 1;
	}
	catch (...)
	{
		std::cerr << "Something went wrong." << std::endl;
		sock::cleanup();
		return 1;
	}
}
