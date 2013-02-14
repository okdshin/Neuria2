#pragma once
//ErrorCode:20130110
#include <iostream>
#include <boost/system/error_code.hpp> 
#include <string>

namespace neuria{
namespace command
{
class ErrorCode{
public:
    ErrorCode(const boost::system::error_code& ec) : message(ec.message()){}
    ErrorCode(const std::string& message) : message(message){}
	
	auto ToString()const -> std::string {
		return this->message;	
	}

private:
	std::string message;

};

inline auto operator<<(std::ostream& os, const ErrorCode& error_code) -> std::ostream& {
	os << error_code.ToString();
	return os;
}
}
}
