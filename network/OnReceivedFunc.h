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
public:
	OnReceivedFunc():on_received([](const Socket::Ptr&, const ByteArray&){}){}
    OnReceivedFunc(boost::function<void (
			const Socket::Ptr&, const ByteArray&)> on_received)
		: on_received(on_received){}

	auto operator()(
			const Socket::Ptr& connection, const ByteArray& byte_array) -> void {
		on_received(connection, byte_array);	
	}
private:
	boost::function<void (const Socket::Ptr&, const ByteArray&)> on_received;
};
}
}
