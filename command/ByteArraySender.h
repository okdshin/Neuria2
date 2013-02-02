#pragma once
//ByteArraySender:20130202
#include <iostream>
#include <boost/function.hpp>
#include "ByteArray.h"
#include "OnSendedFunc.h"
#include "OnFailedFunc.h"

namespace neuria{
namespace command
{
class ByteArraySender{
private:
	using FuncType = boost::function<
			void (const ByteArray&, const OnSendedFunc&, const OnFailedFunc&)>;

public:
    ByteArraySender(FuncType func):func(func){}
    ByteArraySender()
		:func([](const ByteArray&, const OnSendedFunc&, const OnFailedFunc&){}){}

	auto operator()(
			const ByteArray& byte_array, 
			const OnSendedFunc& on_sended,
			const OnFailedFunc& on_failed)const -> void {
		func(byte_array, on_sended, on_failed);
	}

private:
	FuncType func;

};
}
}
