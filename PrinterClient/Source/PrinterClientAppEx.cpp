#include <iostream>

#include "PrinterSerMsg.hpp"
#include "PrinterSerCodec.hpp"
#include "ClientStore.hpp"
#include "SockSocketUtils.hpp"

void print(msg::Client& p_printer, const std::string& p_str)
{
	try
	{
		using Codec = Networking::PrinterSerMsg::Json::Codec;
		Networking::PrinterSerMsg::Print msg = {};
		msg.data = p_str;
		p_printer.sendInd(Codec::encode(msg));
	}
	catch (std::exception& e)
	{
		std::cout << "Error while printing. " << e.what() << std::endl;
	}
}

int main()
{
	try
	{
		sock::init();

		Networking::ClientStore clients;
		auto printerClient = clients.get("PrinterService");
		print(printerClient, "some text");
		print(printerClient, "some other text");
		print(printerClient, "qwddf niiore ertgtttttttttttttttt");
		print(printerClient, "line1\n  line2\nline3");

		using Codec = Networking::PrinterSerMsg::Json::Codec;
		printerClient.sendInd(Codec::encode(Networking::PrinterSerMsg::Stop{}));

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
