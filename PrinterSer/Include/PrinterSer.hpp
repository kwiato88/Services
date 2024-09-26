#pragma once

#include "PrinterSerMsg.hpp"
#include "PrinterSerCodec.hpp"
#include "MsgService.hpp"

namespace Networking
{

using BaseService = msg::Service<PrinterSerMsg::Json::Codec>;
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
