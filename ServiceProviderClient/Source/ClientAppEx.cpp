#include <iostream>

#include "SockSocketUtils.hpp"
#include "MsgTcpIpConnection.hpp"
#include "MsgClient.hpp"
#include "ServiceProviderSerMsg.hpp"
#include "ServiceProviderSerCodec.hpp"

namespace Networking
{

class ServiceProviderClient
{
public:
	ServiceProviderClient(msg::ConnectionFactory p_getConnection);

	ServiceProviderMsg::ServiceAddr getAddr(const std::string& p_service);
	ServiceProviderMsg::ServiceAddr setAddr(const std::string& p_service, const std::string& p_host, const std::string& p_port);
	void stop();
private:
	using JsonCodec = ServiceProviderMsg::Json::Codec;
	msg::Client client;
};

ServiceProviderClient::ServiceProviderClient(msg::ConnectionFactory p_getConnection)
	: client(p_getConnection)
{}

ServiceProviderMsg::ServiceAddr ServiceProviderClient::getAddr(const std::string& p_service)
{
	ServiceProviderMsg::GetServiceAddr req = {};
	req.name = p_service;
	return JsonCodec::decode<ServiceProviderMsg::ServiceAddr>(client.sendReq(JsonCodec::encode(req)));
}

ServiceProviderMsg::ServiceAddr ServiceProviderClient::setAddr(const std::string& p_service, const std::string& p_host, const std::string& p_port)
{
	ServiceProviderMsg::SetService req = {};
	req.name = p_service;
	req.host = p_host;
	req.port = p_port;
	return JsonCodec::decode<ServiceProviderMsg::ServiceAddr>(client.sendReq(JsonCodec::encode(req)));
}

void ServiceProviderClient::stop()
{
	client.sendInd(JsonCodec::encode(ServiceProviderMsg::Stop{}));
}

}

std::unique_ptr<msg::Connection> createConnectionToLocalTcpIpHost()
{
	std::cout << "Create connection to 127.0.0.1:5000" << std::endl;
	return std::make_unique<msg::TcpIpConnection>("127.0.0.1", "50000");
}

void registerSer(Networking::ServiceProviderClient& p_provider, const std::string& p_name, const std::string& p_host, const std::string& p_port)
{
	try
	{
		std::cout << "\nRegister " << p_name << " at [" << p_host << ":" << p_port << "]" << std::endl;
		auto resp = p_provider.setAddr(p_name, p_host, p_port);
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
		auto resp = p_provider.getAddr(p_name);
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

		Networking::ServiceProviderClient serviceProvider(&createConnectionToLocalTcpIpHost);

		getSer(serviceProvider, "MyService");
		registerSer(serviceProvider, "MyService", "127.0.0.1", "1111");
		getSer(serviceProvider, "MyService");

		registerSer(serviceProvider, "MyService", "127.0.0.1", "50005");
		getSer(serviceProvider, "MyService");

		registerSer(serviceProvider, "MyService2", "127.0.0.1", "50001");
		getSer(serviceProvider, "MyService2");

		registerSer(serviceProvider, "MyService3", "127.0.0.1", "50000");
		getSer(serviceProvider, "MyService3");

		std::cout << "\nStop service provider" << std::endl;
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
