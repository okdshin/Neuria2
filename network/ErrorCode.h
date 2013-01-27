#pragma once
//ErrorCode:20130110
#include <iostream>
#include <boost/system/error_code.hpp> 
#include <string>

namespace neuria{
namespace network
{
class ErrorCode{
public:
    ErrorCode(const boost::system::error_code& ec) : message(ec.message()){}
    ErrorCode(std::string message) : message(message){}
	
	friend auto operator<<(std::ostream& os, const ErrorCode& error_code) -> std::ostream&;

private:
	std::string message;

};

auto operator<<(std::ostream& os, const ErrorCode& error_code) -> std::ostream& {
	os << error_code.message;
	return os;
}
}
}
