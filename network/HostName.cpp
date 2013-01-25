#ifdef HOSTNAME_UNIT_TEST
#include "HostName.h"
#include <iostream>

using namespace neuria;
using namespace neuria::network;

int main(int argc, char* argv[])
{
	HostName ip_address(NodeId("192.168.11.1:54321"));
	std::cout << ip_address << std::endl;

    return 0;
}

#endif
