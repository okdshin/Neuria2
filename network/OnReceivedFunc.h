#pragma once
//OnReceivedFunc:20130112
#include <iostream>
#include <boost/function.hpp>
#include "ByteArray.h"
#include "Socket.h"

namespace neuria{
namespace network
{
class OnReceivedFunc{
private:
	using FuncType = boost::function<void (const ByteArray&)>;
public:
	OnReceivedFunc():on_received([](const ByteArray&){}){}
    OnReceivedFunc(FuncType on_received)
		: on_received(on_received){}

	auto operator()(const ByteArray& byte_array)const -> void {
		on_received(byte_array);	
	}
private:
	const FuncType on_received;
};
}
}
