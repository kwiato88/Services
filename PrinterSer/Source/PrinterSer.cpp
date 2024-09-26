#include <iostream>

#include "PrinterSer.hpp"
#include "PrinterSerCodec.hpp"
#include "ServiceProviderClient.hpp"
#include "MsgTcpIpServer.hpp"

namespace Networking
{

class PrintHandler : public msg::IndHandler<PrinterSerMsg::Print>
{
public:
	void handle(const PrinterSerMsg::Print& p_msg)
	{
		std::cout << "\n  PRINT" << std::endl;
		std::cout << p_msg.data << std::endl;
	}
};

Printer::Printer()
	: BaseService(std::bind(&Printer::createServer, this))
{
	ServiceProviderClient provider;
	auto addr = provider.setServiceAddr(name);
	host = addr.host;
	port = addr.port;
	std::cout << name << " registered at [" << host << ":" << port << "]" << std::endl;

	addHandler<PrinterSerMsg::Print>(std::make_shared<PrintHandler>());
	// TODO: stop handler
	//addIndHandler<StopHandler, Codec>(std::make_unique<StopHandler>(*this), BaseService::Processing::Async);
}


Printer::~Printer()
{
	try
	{
		ServiceProviderClient provider;
		provider.removeServiceAddr(name);
	}
	catch (std::exception&)
	{
	}
}

std::unique_ptr<msg::Server> Printer::createServer()
{
	return std::make_unique<msg::TcpIpServer>(host, port);
}

}
