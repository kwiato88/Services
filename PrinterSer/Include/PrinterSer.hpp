#pragma once

#include "MsgService.hpp"

namespace Networking
{

namespace PrinterSerMsg
{
enum class ID
{
	Print,
	Stop,
	Dummy
};

ID getID(const std::string&);

}

using BaseService = msg::Service<PrinterSerMsg::ID>;
class Printer : public BaseService
{
public:
	Printer();

private:
	std::unique_ptr<msg::Server> createServer();

	std::string host;
	std::string port;
};

}
