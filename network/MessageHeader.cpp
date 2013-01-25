#ifdef MESSAGEHEADER_UNIT_TEST
#include "MessageHeader.h"
#include <iostream>

using namespace network;

int main(int argc, char* argv[])
{
	/*
	std::cout << pow(2,4) << std::endl;
	std::cout << pow(2,8) << std::endl;
	std::cout << pow(2,16) << std::endl;
	std::cout << pow(2,32) << std::endl;
	std::cout << pow(2,64) << std::endl;
	std::cout << pow(2,128) << std::endl;
	*/
	{
		MessageHeader header8(pow(2,8)-42);
		std::cout << header8 << std::endl;
		std::cout << header8.Serialize() << std::endl;
		std::cout << MessageHeader::Parse(header8.Serialize()) << std::endl;
		std::cout << "\n";
	}
	{
		MessageHeader header16(pow(2,16)-42);
		std::cout << header16 << std::endl;
		std::cout << header16.Serialize() << std::endl;
		std::cout << MessageHeader::Parse(header16.Serialize()) << std::endl;
		std::cout << "\n";
	}
	{
		MessageHeader header32(pow(2,32)-42);
		std::cout << header32 << std::endl;
		std::cout << header32.Serialize() << std::endl;
		std::cout << MessageHeader::Parse(header32.Serialize()) << std::endl;
		std::cout << "\n";
	}
	{
		MessageHeader header64(pow(2, 62));
		std::cout << header64 << std::endl;
		std::cout << header64.Serialize() << std::endl;
		std::cout << MessageHeader::Parse(header64.Serialize()) << std::endl;
	}
	{
		MessageHeader header64(pow(2, 128));
		std::cout << header64 << std::endl;
		std::cout << header64.Serialize() << std::endl;
		std::cout << MessageHeader::Parse(header64.Serialize()) << std::endl;
	}
    return 0;
}

#endif
