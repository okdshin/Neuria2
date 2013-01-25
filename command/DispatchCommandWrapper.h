#pragma once
//DispatchCommandWrapper:20120913
#include <iostream>
#include <sstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "ByteArray.h"
#include "CommandId.h"

namespace neuria{
namespace command{

class DispatchCommandWrapper{
public: 
	DispatchCommandWrapper(){}
	DispatchCommandWrapper(const CommandId& command_id, const ByteArray& byte_array)
		: command_id_str(command_id.ToString()), byte_array(byte_array){}

	static auto Parse(const ByteArray& byte_array) -> DispatchCommandWrapper {
		std::stringstream ss(CreateStringFromByteArray(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = DispatchCommandWrapper();
		ia >> command;
		return command;
	}
	
	auto Serialize()const -> ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		try{
			oa << static_cast<const DispatchCommandWrapper&>(*this);
		}
		catch(const std::exception& e){
			std::cout << "Error:DispatchCommandWrapper::Serialize:"<< e.what() << std::endl;	
		}
		return CreateByteArrayFromString(ss.str());	
	}

	auto GetCommandId()const -> CommandId { return CommandId(command_id_str); }
	auto GetWrappedByteArray()const -> ByteArray { return byte_array; }

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & command_id_str & byte_array;
	}

	std::string command_id_str;
	ByteArray byte_array;
};

inline auto operator<<(std::ostream& os, const DispatchCommandWrapper& command) -> std::ostream& {
	os << "command_id:\"" << command.GetCommandId().ToString() << "\" byte_array:\"" 
		<< CreateStringFromByteArray(command.GetWrappedByteArray()) << "\"" << std::endl;
	return os;
}

}}
