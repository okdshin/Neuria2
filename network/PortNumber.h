#pragma once
//PortNumber:20130110
#include <iostream>
#include <boost/lexical_cast.hpp>

namespace neuria{
namespace network
{
class PortNumber{
public:
    PortNumber(int port_num) : port_num(port_num){}
    PortNumber(const std::string& port_num_str) 
		: port_num(boost::lexical_cast<int>(port_num_str)){}

	auto ToInt()const -> int {
		return this->port_num;	
	}
	
	auto ToString()const -> std::string {
		return boost::lexical_cast<std::string>(this->port_num);	
	}
private:
	int port_num;
};

auto operator<<(std::ostream& os, const PortNumber& port_num) -> std::ostream& {
	os << "PortNumber:" << port_num.ToInt();
	return os;
}
}
}
