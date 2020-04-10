#include <iostream>

#include "PrinterSerMsg.hpp"
#include "PrinterSerCodec.hpp"
#include "ClientStore.hpp"
#include "MessageSender.hpp"
#include "SockSocketUtils.hpp"

using PrinterCodec = Networking::PrinterSerMsg::Json::Codec;
using PrinterClient = Networking::ClientWithCodec<PrinterCodec>;

void print(PrinterClient& p_printer, const std::string& p_str)
{
	try
	{
		Networking::PrinterSerMsg::Print msg = {};
		msg.data = p_str;
		p_printer.sendInd(msg);
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
		auto printerClient = clients.getWithCodec<PrinterCodec>("PrinterService");
		print(printerClient, "some text");
		print(printerClient, "some other text");
		print(printerClient, "qwddf niiore ertgtttttttttttttttt");
		print(printerClient, "line1\n  line2\nline3");

		Networking::MessageSender sender;
		sender.sendInd<PrinterCodec>("PrinterService", Networking::PrinterSerMsg::Print{ "qwerty" });

		using Codec = Networking::PrinterSerMsg::Json::Codec;
		printerClient.sendInd(Networking::PrinterSerMsg::Stop{});

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
