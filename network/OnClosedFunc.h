#pragma once
//OnClosedFunc:20130112
#include <iostream>
#include <boost/function.hpp>
#include "ByteArray.h"

namespace network
{
class OnClosedFunc{
public:
	OnClosedFunc():on_closed([](){
			std::cout << "Called(Default)OnClosedFunc" << std::endl;
		
		}){}
    OnClosedFunc(boost::function<void ()> on_closed)
		: on_closed(on_closed){}
    ~OnClosedFunc(){}

	auto operator()() -> void {
		on_closed();	
	}
private:
	boost::function<void ()> on_closed;
};
}

