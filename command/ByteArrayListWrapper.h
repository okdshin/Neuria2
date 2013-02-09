#pragma once
//ByteArrayListWrapper:20120913
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

class ByteArrayListWrapper{
public: 
	ByteArrayListWrapper(){}

	static auto Parse(const ByteArray& byte_array) -> ByteArrayListWrapper {
		try{
			std::stringstream ss(CreateStringFromByteArray(byte_array));
			boost::archive::text_iarchive ia(ss);
			auto command = ByteArrayListWrapper();
			ia >> command;
			return command;
		}catch(const std::exception& e){
			std::cout << "ByteArrayListWrapper Parse Error!!\n" 
				<< e.what() << "\n" << CreateStringFromByteArray(byte_array) 
				<< std::endl;
			throw e;	
		}
	}
	
	auto Serialize()const -> ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		try{
			oa << static_cast<const ByteArrayListWrapper&>(*this);
		}
		catch(const std::exception& e){
			std::cout << "Error:ByteArrayListWrapper::Serialize:"<< e.what() << std::endl;	
			throw e;
		}
		return CreateByteArrayFromString(ss.str());	
	}

	auto PushBack(const ByteArray& byte_array) -> void { 
		return byte_array_list.push_back(byte_array); 
	}

	auto At(unsigned int index)const -> ByteArray{
		return byte_array_list.at(index);
	}

	auto Size()const -> unsigned int {	
		return this->byte_array_list.size();
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & byte_array_list;
	}

	std::vector<ByteArray> byte_array_list;
};

inline auto operator<<(
		std::ostream& os, const ByteArrayListWrapper& wrapper) -> std::ostream& {
	for(unsigned int i = 0; i < wrapper.Size(); ++i){
		os << CreateStringFromByteArray(wrapper.At(i)) << ",";
	}
	return os;
}

}}
