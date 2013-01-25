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
	
	auto operator<<(std::ostream& os)const -> std::ostream& {
		os << this->message;
		return os;
	}

private:
	std::string message;

};

}
}
