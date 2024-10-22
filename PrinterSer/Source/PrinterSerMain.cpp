#include <iostream>

#include "PrinterSer.hpp"
#include "SockSocketUtils.hpp"

int main()
{
	try
	{
		sock::init();
		
		{
			std::cout << "Printer: Create service" << std::endl;
			Networking::Printer printer;
			printer.start();
		}
		sock::cleanup();
		return 0;
	}
	catch (std::exception& e)
	{
		std::cout << "Printer: Something went wrong: " << e.what() << std::endl;
		sock::cleanup();
		return 1;
	}
}
