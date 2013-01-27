#ifdef SOCKET_UNIT_TEST
#include "Socket.h"
#include <iostream>

using namespace neuria;
using namespace neuria::network;

int main(int argc, char* argv[])
{
	auto io_service = IoService::Create();
	auto socket = Socket::Create(io_service);

    return 0;
}

#endif
