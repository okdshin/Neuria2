#pragma once
//OnReceivedFunc:20130112
#include <iostream>
#include <boost/function.hpp>
#include "ByteArray.h"

namespace neuria{
namespace command
{
class OnReceivedFunc{
public:
	OnReceivedFunc():on_received([](const ByteArray&){}){}
    OnReceivedFunc(boost::function<void (const ByteArray&)> on_received)
		: on_received(on_received){}
    ~OnReceivedFunc(){}

	auto operator()(const ByteArray& byte_array) -> void {
		on_received(byte_array);	
	}
private:
	boost::function<void (const ByteArray&)> on_received;
};
}
}
