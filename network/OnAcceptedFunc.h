#pragma once
//OnAcceptedFunc:20130112
#include <iostream>
#include <boost/function.hpp>
#include "ByteArray.h"
#include "Socket.h"

namespace neuria{
namespace network
{
class OnAcceptedFunc{
public:
	OnAcceptedFunc():on_accepted([](Socket::Ptr){}){}
    OnAcceptedFunc(boost::function<void (Socket::Ptr)> on_accepted)
		: on_accepted(on_accepted){}
    ~OnAcceptedFunc(){}

	auto operator()(Socket::Ptr connection) -> void {
		on_accepted(connection);	
	}
private:
	boost::function<void (Socket::Ptr)> on_accepted;
};
}
}
