#pragma once
//Message:20130112
#include <iostream>
#include "MessageHeader.h"
#include "MessageBody.h"

namespace neuria{
namespace network
{
class Message{
public:
	Message():header(), body(){}
    Message(const MessageHeader& header, const MessageBody& body) 
		: header(header), body(body){}

	auto ToByteArray()const -> ByteArray {
		auto header_byte_array = header.Serialize();
		auto body_byte_array = body.ToByteArray();
		std::copy(
			body_byte_array.begin(), body_byte_array.end(), 
			std::back_inserter(header_byte_array));
		return header_byte_array;
	}

private:
	MessageHeader header;
	MessageBody body;

};
}
}
