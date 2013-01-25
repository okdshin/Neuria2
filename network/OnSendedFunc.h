#pragma once
//OnSendedFunc:20130112
#include <iostream>
#include <boost/function.hpp>

namespace neuria{
namespace network
{
class OnSendedFunc{
public:
	OnSendedFunc():on_sended([](){
			std::cout << "Called(Default)OnSendedFunc" << std::endl;
		}){}
    OnSendedFunc(boost::function<void ()> on_sended) : on_sended(on_sended){}
    ~OnSendedFunc(){}

	auto operator()() -> void {
		this->on_sended();	
	}

private:
	boost::function<void ()> on_sended;

};
}
}
