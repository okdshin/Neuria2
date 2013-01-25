#pragma once
//MessageBody:20130112
#include <iostream>
#include "ByteArray.h"

namespace neuria{
namespace network
{
class MessageBody{
public:
    MessageBody(const ByteArray& byte_array) : byte_array(byte_array){}
    ~MessageBody(){}

	auto ToByteArray()const -> ByteArray {
		return this->byte_array;	
	}

private:
	ByteArray byte_array;

};
}
}
