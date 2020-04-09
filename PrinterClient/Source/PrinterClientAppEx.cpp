#include <iostream>

#include "PrinterSer.hpp"
#include "ServiceProviderClient.hpp"
#include "SockSocketUtils.hpp"
#include "MsgTcpIpConnection.hpp"

namespace Networking
{

class ClientStore
{
public:
	msg::Client get(const std::string& p_serviceName);

private:
	Networking::ServiceProviderClient addrs;
};

msg::Client ClientStore::get(const std::string& p_serviceName)
{
	auto printerAddr = addrs.getServiceAddr(p_serviceName);
	return msg::Client([=]() { return std::make_unique<msg::TcpIpConnection>(printerAddr.host, printerAddr.port); });
}

}

int main()
{
	try
	{
		sock::init();

		Networking::ClientStore clients;
		auto printerClient = clients.get("PrinterService");
		printerClient.sendInd("some text");
		printerClient.sendInd("some other text");
		printerClient.sendInd("qwddf niiore ertgtttttttttttttttt");
		printerClient.sendInd("line1\n  line2\nline3");
		printerClient.sendInd("Stop");

		sock::cleanup();
		return 0;
	}
	catch (std::exception& e)
	{
		std::cout << "Somethin went wrong: " << e.what() << std::endl;
		sock::cleanup();
		return 1;
	}
}