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
	return ID::Stop;
}
}

Printer::Printer()
	: BaseService(std::bind(&Printer::createServer, this), &PrinterSerMsg::getID)
{
	ServiceProviderClient provider;
	auto addr = provider.setServiceAddr("PrinterService");
	host = addr.host;
	port = addr.port;
	std::cout << "PrinterService registered at [" << host << ":" << port << "]";
}

std::unique_ptr<msg::Server> Printer::createServer()
{
	return std::make_unique<msg::TcpIpServer>(host, port);
}

}
