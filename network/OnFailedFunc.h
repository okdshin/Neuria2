#pragma once
//OnFailedFunc:20130112
#include <iostream>
#include <boost/function.hpp>
#include "ErrorCode.h"

namespace neuria{
namespace network
{
class OnFailedFunc{
public:
	OnFailedFunc()
		:on_failed([](const ErrorCode& error_code){ 
			std::cout << "Called(Default)OnFailedFunc:" << error_code << std::endl;
		}){}
    OnFailedFunc(boost::function<void (const ErrorCode&)> on_failed) 
		: on_failed(on_failed){}
    ~OnFailedFunc(){}

	auto operator()(const ErrorCode& error_code) -> void {
		on_failed(error_code);	
	} 

private:
	boost::function<void (const ErrorCode&)> on_failed;

};
}
}
