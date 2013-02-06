#pragma once
//ConnectionCloser:20130202
#include <iostream>
#include <boost/function.hpp>
#include "ByteArray.h"
#include "OnSendedFunc.h"
#include "OnFailedFunc.h"

namespace neuria{
namespace command
{
class ConnectionCloser{
private:
	using FuncType = boost::function<void ()>;

public:
    ConnectionCloser(FuncType func):func(func){}
    ConnectionCloser()
		:func([](){}){}

	auto operator()()const -> void {
		func();
	}

private:
	FuncType func;

};
}
}
