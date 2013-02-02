#pragma once
//OnFailedFunc:20130112
#include <iostream>
#include <boost/function.hpp>
#include "ErrorCode.h"

namespace neuria{
namespace command
{
class OnFailedFunc{
private:
	using FuncType = boost::function<void (const ErrorCode&)>;
public:
	OnFailedFunc()
		:on_failed([](const ErrorCode& error_code){ 
			std::cout << "Called(Default)OnFailedFunc:" << error_code << std::endl;
		}){}
    OnFailedFunc(FuncType on_failed) 
		: on_failed(on_failed){}

	auto operator()(const ErrorCode& error_code) -> void {
		on_failed(error_code);	
	} 

private:
	FuncType on_failed;

};
}
}
