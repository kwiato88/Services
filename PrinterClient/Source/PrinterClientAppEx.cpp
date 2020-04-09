#include <iostream>

#include "PrinterSer.hpp"
#include "ClientStore.hpp"
#include "SockSocketUtils.hpp"

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