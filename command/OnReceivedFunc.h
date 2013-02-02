#pragma once
//OnReceivedFunc:20130112
#include <iostream>
#include <boost/function.hpp>
#include "ByteArraySender.h"

namespace neuria{
namespace command
{
class OnReceivedFunc{
private:
	using FuncType = 
		boost::function<void (const ByteArraySender&, const ByteArray&)>;

public:
    OnReceivedFunc() : on_received(
		[](const ByteArraySender&, const ByteArray&){}){}
    OnReceivedFunc(FuncType on_received) : on_received(on_received){}

	auto operator()(
			const ByteArraySender& sender, 
			const ByteArray& received_byte_array) -> void {
		on_received(sender, received_byte_array);	
	}
private:
	FuncType on_received;
};
}
}
