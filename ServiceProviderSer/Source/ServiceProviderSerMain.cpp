#include <iostream>

#include "ServiceProviderSer.hpp"
#include "SockSocketUtils.hpp"
#include "MsgTcpIpServer.hpp"

std::unique_ptr<msg::Server> createLocalTcpIpServer()
{
	std::cout << "ServiceProvider: Create server on 127.0.0.1:50000" << std::endl;
	return std::make_unique<msg::TcpIpServer>("127.0.0.1", "50000");
}

int main()
{
	try
	{
		sock::init();
		Networking::ServiceProvider server(&createLocalTcpIpServer);
		std::cout << "ServiceProvider: Start" << std::endl;
		server.start();
		std::cout << "ServiceProvider: finished" << std::endl;
		sock::cleanup();
		return 0;
	}
	catch (std::exception& e)
	{
		std::cerr << "ServiceProvider: Something went wrong. Exception: " << e.what() << std::endl;
		sock::cleanup();
		return 1;
	}
	catch (...)
	{
		std::cerr << "ServiceProvider: Something went wrong." << std::endl;
		sock::cleanup();
		return 1;
	}
}
