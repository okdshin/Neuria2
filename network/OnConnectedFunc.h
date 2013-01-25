#pragma once
//OnConnectedFunc:20130112
#include <iostream>
#include <boost/function.hpp>
#include "ByteArray.h"
#include "Socket.h"

namespace neuria{
namespace network
{
class OnConnectedFunc{
public:
	OnConnectedFunc():on_connected([](Socket::Ptr){}){}
    OnConnectedFunc(boost::function<void (Socket::Ptr)> on_connected)
		: on_connected(on_connected){}
    ~OnConnectedFunc(){}

	auto operator()(Socket::Ptr connection) -> void {
		on_connected(connection);	
	}
private:
	boost::function<void (Socket::Ptr)> on_connected;
};
}
}
