#pragma once

#include <stdexcept>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace JsonCodec
{

struct CodecError : public std::runtime_error
{
	CodecError(const std::string& p_what) : std::runtime_error(p_what) {}
};

template<typename IdConverter, typename Serialization>
struct Codec
{
	using ExceptionType = CodecError;
	using IdType = typename IdConverter::IdType;

	static IdType idFromString(const std::string& p_data)
	{
		return IdConverter::fromString(p_data);
	}

	static std::string idToString(IdType p_id)
	{
		return IdConverter::toString(p_id);
	}

	static IdType getId(const std::string& p_data)
	{
		try
		{
			std::stringstream buff;
			buff << p_data;
			boost::property_tree::ptree data;
			boost::property_tree::json_parser::read_json(buff, data);
			return idFromString(data.get<std::string>("ID", ""));
		}
		catch (std::exception&)
		{
			return IdConverter::deafultId;
		}
	}

	static std::string encode_Message(IdType p_id, const boost::property_tree::ptree& p_data)
	{
		boost::property_tree::ptree data;
		data.put("ID", idToString(p_id));
		data.add_child("msg", p_data);

		std::stringstream buffer;
		boost::property_tree::json_parser::write_json(buffer, data);
		return buffer.str();
	}

	static boost::property_tree::ptree decode_Message(IdType p_expectedId, const std::string& p_data)
	{
		std::stringstream buff;
		buff << p_data;
		boost::property_tree::ptree data;
		boost::property_tree::json_parser::read_json(buff, data);

		if (p_expectedId != idFromString(data.get<std::string>("ID", "")))
			throw ExceptionType(std::string("Unexpected ID. Expected: ") + idToString(p_expectedId) + ". Got: " + data.get<std::string>("ID", ""));
		return data.get_child("msg");
	}

	template<typename T>
	static std::string encode(const T& p_msg)
	{
		try
		{
			return encode_Message(T::id, Serialization::template toPtree<T>(p_msg));
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
			return Serialization::template fromPtree<T>(decode_Message(T::id, p_msg));
		}
		catch (std::exception& e)
		{
			throw ExceptionType(e.what());
		}
	}
};

}
