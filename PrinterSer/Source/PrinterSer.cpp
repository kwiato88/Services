#include <iostream>

#include "PrinterSer.hpp"
#include "ServiceProviderClient.hpp"
#include "MsgTcpIpServer.hpp"

namespace Networking
{
namespace PrinterSerMsg
{
ID getID(const std::string& p_msg)
{
	if(p_msg == "Stop")
		return ID::Stop;
	return ID::Print;
}
}

class PrintHandler : public msg::Handler
{
public:
	std::string handle(const std::string& p_msg)
	{
		std::cout << "  PRINT" << std::endl;
		std::cout << p_msg << std::endl;
		return "";
	}
};

Printer::Printer()
	: BaseService(std::bind(&Printer::createServer, this), &PrinterSerMsg::getID)
{
	ServiceProviderClient provider;
	auto addr = provider.setServiceAddr(name);
	host = addr.host;
	port = addr.port;
	std::cout << name << " registered at [" << host << ":" << port << "]" << std::endl;

	addHandler(PrinterSerMsg::ID::Stop, std::make_unique<BaseService::NativeStopHandler>(*this));
	addHandler(PrinterSerMsg::ID::Print, std::make_unique<PrintHandler>());
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
