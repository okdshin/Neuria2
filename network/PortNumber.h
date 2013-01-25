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
    ~PortNumber(){}

	auto ToInt()const -> int {
		return this->port_num;	
	}
	
	auto ToString()const -> std::string {
		return boost::lexical_cast<std::string>(this->port_num);	
	}
private:
	int port_num;

};
}
}
