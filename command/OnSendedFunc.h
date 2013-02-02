#pragma once
//OnSendedFunc:20130112
#include <iostream>
#include <boost/function.hpp>

namespace neuria{
namespace command
{
class OnSendedFunc{
private:
	using FuncType = boost::function<void ()>;
public:
	OnSendedFunc():on_sended([](){
			std::cout << "Called(Default)OnSendedFunc" << std::endl;
		}){}

    OnSendedFunc(FuncType on_sended) : on_sended(on_sended){}

	auto operator()() -> void {
		this->on_sended();	
	}

private:
	FuncType on_sended;

};
}
}
