#pragma once

#include "PrinterSerMsg.hpp"
#include "MsgService.hpp"

namespace Networking
{

using BaseService = msg::Service<PrinterSerMsg::ID>;
class Printer : public BaseService
{
public:
	Printer();
	~Printer();

private:
	std::unique_ptr<msg::Server> createServer();

	std::string host;
	std::string port;
	const std::string name = "PrinterService";
};

}
