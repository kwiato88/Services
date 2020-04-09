#pragma once

#include "ServiceProviderSerMsg.hpp"
#include <stdexcept>

namespace Networking
{
namespace ServiceProviderMsg
{
namespace Json
{

struct CodecError : public std::runtime_error
{
	CodecError(const std::string& p_what) : std::runtime_error(p_what) {}
};

ID getId(const std::string&);
std::string    encode_GetServiceAddr(const GetServiceAddr&);
GetServiceAddr decode_GetServiceAddr(const std::string&);
std::string encode_ServiceAddr(const ServiceAddr&);
ServiceAddr decode_ServiceAddr(const std::string&);
std::string encode_SetService(const SetService&);
SetService  decode_SetService(const std::string&);
std::string   encode_RemoveService(const RemoveService&);
RemoveService decode_RemoveService(const std::string&);
std::string encode_Stop(const Stop&);
Stop        decode_Stop(const std::string&);


struct Codec
{
	using ExceptionType = Networking::ServiceProviderMsg::Json::CodecError;
	using Id = ServiceProviderMsg::ID;

private:
	template<typename ParameterT, typename ReturnT>
	static ReturnT withThrow(ReturnT(*function)(const ParameterT&), const ParameterT& param)
	{
		try
		{
			return (*function)(param);
		}
		catch (std::exception& e)
		{
			throw ExceptionType(e.what());
		}
	}

public:
	static Id getId(const std::string& p_msg)
	{
		try
		{
			return Networking::ServiceProviderMsg::Json::getId(p_msg);
		}
		catch (std::exception&)
		{
			return Id::Dummy;
		}
	}

	template<typename T>
	static std::string encodeImpl(const T&)
	{
		return "";
	}
	template<typename T>
	static T decodeImpl(const std::string&)
	{
		return T{};
	}

	template<typename T>
	static std::string encode(const T& p_msg)
	{
		try
		{
			return encodeImpl<T>(p_msg);
		}
		catch (std::exception& e)
		{
			throw ExceptionType(e.what());
		}
	}
	template<typename T>
	static T decode(const std::string& p_msg)
	{
		try
		{
			return decodeImpl<T>(p_msg);
		}
		catch (std::exception& e)
		{
			throw ExceptionType(e.what());
		}
	}

	template<>
	static std::string encodeImpl<ServiceProviderMsg::SetService>(const ServiceProviderMsg::SetService& p_msg)
	{
		return Networking::ServiceProviderMsg::Json::encode_SetService(p_msg);
	}
	template<>
	static ServiceProviderMsg::SetService decodeImpl<ServiceProviderMsg::SetService>(const std::string& p_msg)
	{
		return Networking::ServiceProviderMsg::Json::decode_SetService(p_msg);
	}

	template<>
	static std::string encodeImpl<ServiceProviderMsg::RemoveService>(const ServiceProviderMsg::RemoveService& p_msg)
	{
		return Networking::ServiceProviderMsg::Json::encode_RemoveService(p_msg);
	}
	template<>
	static ServiceProviderMsg::RemoveService decodeImpl<ServiceProviderMsg::RemoveService>(const std::string& p_msg)
	{
		return Networking::ServiceProviderMsg::Json::decode_RemoveService(p_msg);
	}

	template<>
	static std::string encodeImpl<ServiceProviderMsg::ServiceAddr>(const ServiceProviderMsg::ServiceAddr& p_msg)
	{
		return Networking::ServiceProviderMsg::Json::encode_ServiceAddr(p_msg);
	}
	template<>
	static ServiceProviderMsg::ServiceAddr decodeImpl<ServiceProviderMsg::ServiceAddr>(const std::string& p_msg)
	{
		return Networking::ServiceProviderMsg::Json::decode_ServiceAddr(p_msg);
	}

	template<>
	static std::string encodeImpl<ServiceProviderMsg::GetServiceAddr>(const ServiceProviderMsg::GetServiceAddr& p_msg)
	{
		return Networking::ServiceProviderMsg::Json::encode_GetServiceAddr(p_msg);
	}
	template<>
	static ServiceProviderMsg::GetServiceAddr decodeImpl<ServiceProviderMsg::GetServiceAddr>(const std::string& p_msg)
	{
		return Networking::ServiceProviderMsg::Json::decode_GetServiceAddr(p_msg);
	}

	template<>
	static std::string encodeImpl<ServiceProviderMsg::Stop>(const ServiceProviderMsg::Stop& p_msg)
	{
		return Networking::ServiceProviderMsg::Json::encode_Stop(p_msg);
	}
	template<>
	static ServiceProviderMsg::Stop decodeImpl<ServiceProviderMsg::Stop>(const std::string& p_msg)
	{
		return Networking::ServiceProviderMsg::Json::decode_Stop(p_msg);
	}
};

}
}
}
